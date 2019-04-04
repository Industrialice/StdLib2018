#include "_PreHeader.hpp"
#include "File.hpp"
#include "FileSystem.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError();

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, FileShareModes::FileShareMode shareMode)
{
    Close();

    offset = std::min<ui64>(offset, i64_max);

    int flags = 0;

    if (procMode.Contains(FileProcModes::Read) && procMode.Contains(FileProcModes::Write))
    {
        flags |= O_RDWR;
    }
    else if (procMode.Contains(FileProcModes::Read))
    {
        flags |= O_RDONLY;
    }
    else if (procMode.Contains(FileProcModes::Write))
    {
        flags |= O_WRONLY;
    }
    else
    {
        return DefaultError::InvalidArgument("No read or write was requested");
    }

    if (openMode == FileOpenMode::CreateIfNotExist)
    {
        flags |= O_CREAT;
    }
    else if (openMode == FileOpenMode::CreateAlways || openMode == FileOpenMode::CreateNew)
    {
        if (offset > 0)
        {
            return DefaultError::InvalidArgument("'offset' can't be more than 0 when OpenMode::CreateAlways or OpenMode::CreateNew is used");
        }

        if (openMode == FileOpenMode::CreateAlways)
        {
            if (!procMode.Contains(FileProcModes::Write)) // we need to truncate file, but we can't do it if we can't write to it, so we just remove it
            {
                auto removeResult = FileSystem::Remove(pnn);
                if (removeResult)
                {
                    return removeResult;
                }
                flags |= O_CREAT;
            }
            else
            {
                flags |= O_CREAT | O_TRUNC;
            }
        }
        else
        {
            flags |= O_CREAT | O_EXCL;
        }
    }
    else
    {
        ASSUME(openMode == FileOpenMode::OpenExisting);
    }

    if (cacheMode.Contains(FileCacheModes::DisableSystemWriteCache))
    {
        if (!procMode.Contains(FileProcModes::Write))
        {
            return DefaultError::InvalidArgument("FileCacheModes::DisableSystemWriteCache must be used only when FileProcModes::Write is specified");
        }
        flags |= O_DIRECT;
    }
    if (cacheMode.Contains(FileCacheModes::DisableSystemReadCache))
    {
        if (!procMode.Contains(FileProcModes::Read))
        {
            return DefaultError::InvalidArgument("FileCacheModes::DisableSystemReadCache must be used only when FileProcModes::Read is specified");
        }
        flags |= O_DIRECT;
    }

    mode_t processMask = umask(0);
    int hfile = open(pnn.PlatformPath().data(), flags, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (hfile == -1)
    {
        return StdLib_FileError();
    }

    if (cacheMode.Contains(FileCacheModes::LinearRead) || cacheMode.Contains(FileCacheModes::RandomRead))
    {
        if (cacheMode.Contains(FileCacheModes::LinearRead.Combined(FileCacheModes::RandomRead)))
        {
            close(hfile);
            return DefaultError::InvalidArgument("Both FileCacheModes::LinearRead and FileCacheModes::RandomRead are specified");
        }

        if (!procMode.Contains(FileProcModes::Read))
        {
            close(hfile);
            return DefaultError::InvalidArgument("FileCacheModes::LinearRead or FileCacheModes::RandomRead must be used only when FileProcModes::Read is specified");
        }

    #if (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 600)) || (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L))
        if (cacheMode.Contains(FileCacheModes::LinearRead))
        {
            int adviceResult = posix_fadvise(hfile, 0, 0, POSIX_FADV_SEQUENTIAL);
            ASSUME(adviceResult == 0);
        }
        else
        {
            int adviceResult = posix_fadvise(hfile, 0, 0, POSIX_FADV_RANDOM);
            ASSUME(adviceResult == 0);
        }
    #endif
    }

    if (offset > 0)
    {
        struct stat64 stats;
        if (fstat64(hfile, &stats) != 0)
        {
            close(hfile);
            return StdLib_FileError();
        }
        _offsetToStart = std::min<i64>(stats.st_size, offset);
        if (lseek64(hfile, _offsetToStart, SEEK_SET) == -1)
        {
            close(hfile);
            return StdLib_FileError();
        }
    }
    else
    {
        _offsetToStart = 0;
    }

#ifdef STDLIB_ENABLE_FILE_STATS
    _stats = {};
#endif
    _openMode = openMode;
    _procMode = procMode;
    _cacheMode = cacheMode;
    _handle = hfile;
    _bufferPos = 0;
    _readBufferCurrentSize = 0;
    _isOwningFileHandle = true;

    return DefaultError::Ok();
}

Error<> File::Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

Result<FilePath> File::PNN() const
{
    ASSUME(IsOpened());
    char proc[MaxPathLength];
    snprintf(proc, CountOf(proc), "/proc/self/fd/%i", _handle);
    char buf[MaxPathLength];
    ssize_t len = readlink(proc, buf, MaxPathLength - 1);
    if (len == -1)
    {
        return DefaultError::UnknownError("readlink failed");
    }
    return FilePath::FromChar(buf);
}

void File::Close()
{
    if (!IsOpened())
    {
        return;
    }
    PerformFlush(false);
    if (_isOwningFileHandle)
    {
        int result = close(_handle);
        ASSUME(result == 0);
    }
    _handle = -1;
    _bufferPos = 0;
    _readBufferCurrentSize = 0;
}

Result<i64> File::OffsetSet(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpened());

    if (!CancelCachedRead() || !PerformFlush(false)) // TODO: optimize
    {
        return DefaultError::UnknownError();
    }

    int whence;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        if (offset < 0)
        {
            return DefaultError::InvalidArgument("Negative offset value cannot be used with FileOffsetMode::FromBegin");
        }
        offset += _offsetToStart;
        whence = SEEK_SET;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        whence = SEEK_CUR;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        if (offset > 0)
        {
            return DefaultError::InvalidArgument("Positive offset value cannot be used with FileOffsetMode::FromEnd");
        }
        whence = SEEK_END;
    }

    off64_t result = lseek64(_handle, offset, whence);
    if (result == -1)
    {
        return StdLib_FileError();
    }

    if (result < _offsetToStart)
    {
        result = _offsetToStart;
        result = lseek64(_handle, _offsetToStart, SEEK_SET);
        if (result == -1)
        {
            return StdLib_FileError();
        }
    }

    ASSUME(result >= _offsetToStart);
    return result - _offsetToStart;
}

Result<ui64> File::SizeGet()
{
    ASSUME(IsOpened());
    if (!PerformFlush(false)) // flushing first because file pointer may not be at the end of the file, in that case we can't just return FileSize + BufferSize
    {
        return DefaultError::UnknownError();
    }
    struct stat64 stats;
    if (fstat64(_handle, &stats) != 0)
    {
        return StdLib_FileError();
    }
    ASSUME(stats.st_size >= _offsetToStart);
    return stats.st_size - _offsetToStart;
}

Error<> File::SizeSet(ui64 newSize)
{
    ASSUME(IsOpened());

    if (!CancelCachedRead() || !PerformFlush(false))
    {
        return DefaultError::UnknownError();
    }

    newSize += (ui64)_offsetToStart;
    if (newSize < (ui64)_offsetToStart) // overflow
    {
        newSize = ui64_max;
    }

    if (ftruncate(_handle, newSize) != 0)
    {
        return DefaultError::UnknownError("ftrancate failed");
    }

    return DefaultError::Ok();
}

void File::FlushSystemCaches()
{
    ASSUME(IsOpened());
    int result = fsync(_handle);
    ASSUME(result == 0);
}

bool File::WriteToFile(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ASSUME(source || len == 0);

#ifdef STDLIB_ENABLE_FILE_STATS
    ++_stats.writesToFileCount;
#endif

    ssize_t sswritten = write(_handle, source, len);
    if (sswritten == (ssize_t)-1)
    {
        if (written) *written = 0;
        return false;
    }

#ifdef STDLIB_ENABLE_FILE_STATS
    _stats.bytesToFileWritten += sswritten;
#endif

    if (written) *written = (ui32)sswritten;
    return true;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *readRet)
{
    ASSUME(IsOpened());
    ASSUME(target || len == 0);

#ifdef STDLIB_ENABLE_FILE_STATS
    ++_stats.readsFromFileCount;
#endif

    ssize_t actuallyRead = read(_handle, target, len);
    if (actuallyRead == (ssize_t)-1)
    {
        if (readRet) *readRet = 0;
        return false;
    }

#ifdef STDLIB_ENABLE_FILE_STATS
    _stats.bytesFromFileRead += actuallyRead;
#endif

    if (readRet) *readRet = (ui32)actuallyRead;
    return true;
}

NOINLINE bool File::CancelCachedRead()
{
    ASSUME(IsOpened());
    if (_bufferPos >= _readBufferCurrentSize)
    {
        ASSUME(_readBufferCurrentSize == 0 || _bufferPos == _readBufferCurrentSize);
        return true;
    }
    i32 move = (i32)_bufferPos - (i32)_readBufferCurrentSize;
    off64_t result = lseek64(_handle, move, SEEK_CUR);
    _bufferPos = _readBufferCurrentSize = 0;
    return result != -1;
}

Result<i64> File::CurrentFileOffset() const
{
    off64_t offset = lseek64(_handle, 0, SEEK_CUR);
    if (offset == -1)
    {
        return StdLib_FileError();
    }
    ASSUME(offset >= _offsetToStart);
    return offset;
}

namespace StdLib::FileInitialization
{
    void Initialize()
    {}
}