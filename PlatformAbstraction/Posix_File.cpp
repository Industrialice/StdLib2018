#include <StdCoreLib.hpp>
#include "File.hpp"
#include "FileSystem.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError();

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode, FileShareMode shareMode)
{
    Close();

    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("Path is invalid");
    }

    int flags = 0;

    if ((procMode && FileProcMode::Read) && (procMode && FileProcMode::Write))
    {
        flags |= O_RDWR;
    }
    else if (procMode && FileProcMode::Read)
    {
        flags |= O_RDONLY;
    }
    else if (procMode && FileProcMode::Write)
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
        if (procMode && FileProcMode::WriteAppend)
        {
            return DefaultError::InvalidArgument("FileProcMode::Append can't be used with FileOpenMode::CreateAlways or FileOpenMode::CreateNew");
        }

        if (openMode == FileOpenMode::CreateAlways)
        {
            if (!(procMode && FileProcMode::Write)) // we need to truncate file, but we can't do it if we can't write to it, so we just remove it
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

    if (cacheMode && FileCacheMode::DisableSystemWriteCache)
    {
        if (!(procMode && FileProcMode::Write))
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemWriteCache must be used only when FileProcMode::Write is specified");
        }
        flags |= O_DIRECT;
    }
    if (cacheMode && FileCacheMode::DisableSystemReadCache)
    {
        if (!(procMode && FileProcMode::Read))
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemReadCache must be used only when FileProcMode::Read is specified");
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

    if ((cacheMode && FileCacheMode::LinearRead) || (cacheMode &&FileCacheMode::RandomRead))
    {
        if (cacheMode && (FileCacheMode::LinearRead + FileCacheMode::RandomRead))
        {
            close(hfile);
            return DefaultError::InvalidArgument("Both FileCacheMode::LinearRead and FileCacheMode::RandomRead are specified");
        }

        if (!(procMode && FileProcMode::Read))
        {
            close(hfile);
            return DefaultError::InvalidArgument("FileCacheMode::LinearRead or FileCacheMode::RandomRead must be used only when FileProcMode::Read is specified");
        }

    #if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
        if (cacheMode && FileCacheMode::LinearRead)
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

    if (procMode && FileProcMode::WriteAppend)
    {
        int seekResult = lseek64(hfile, 0, SEEK_END);
        if (seekResult == -1)
        {
            close(hfile);
            return StdLib_FileError();
        }
        _offsetToStart = seekResult;
    }

#if ENABLE_FILE_STATS
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

Error<> File::Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

Result<FilePath> File::PNN() const
{
    ASSUME(IsOpened());
    char proc[MaxPathLength];
    snprintf(proc, Funcs::CountOf(proc), "/proc/self/fd/%i", _handle);
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
    Flush();
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

    if (!CancelCachedRead() || !File::Flush()) // TODO: optimize
    {
        return DefaultError::UnknownError();
    }

    int whence;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        ASSUME(offset >= 0);
        offset += _offsetToStart;
        whence = SEEK_SET;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        whence = SEEK_CUR;
    }
    else
    {
        ASSUME(offset <= 0);
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        whence = SEEK_END;
    }

    off64_t result = lseek64(_handle, offset, whence);
    if (result == -1)
    {
        return StdLib_FileError();
    }

    if (_procMode && FileProcMode::WriteAppend)
    {
        if (offsetMode != FileOffsetMode::FromBegin)
        {
            if (result < _offsetToStart)
            {
                result = _offsetToStart;
                result = lseek64(_handle, _offsetToStart, SEEK_SET);
                if (result == -1)
                {
                    return StdLib_FileError();
                }
            }
        }
        ASSUME(result >= _offsetToStart);
        result -= _offsetToStart;
    }

    return result;
}

Result<ui64> File::SizeGet()
{
    ASSUME(IsOpened());
    if (!File::Flush()) // flushing first because file pointer may not be at the end of the file, in that case we can't just return FileSize + BufferSize
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

    if (!CancelCachedRead() || !File::Flush())
    {
        return DefaultError::UnknownError();
    }

    newSize += _offsetToStart;
    if (newSize < _offsetToStart) // overflow
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

#if ENABLE_FILE_STATS
    ++_stats.writesToFileCount;
#endif

    ssize_t sswritten = write(_handle, source, len);
    if (sswritten == (ssize_t)-1)
    {
        if (written) *written = 0;
        return false;
    }

#if ENABLE_FILE_STATS
    _stats.bytesToFileWritten += sswritten;
#endif

    if (written) *written = (ui32)sswritten;
    return true;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *readRet)
{
    ASSUME(IsOpened());
    ASSUME(target || len == 0);

#if ENABLE_FILE_STATS
    ++_stats.readsFromFileCount;
#endif

    ssize_t actuallyRead = read(_handle, target, len);
    if (actuallyRead == (ssize_t)-1)
    {
        if (readRet) *readRet = 0;
        return false;
    }

#if ENABLE_FILE_STATS
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
    ASSUME(offset >= (i64)_offsetToStart);
    return offset;
}

namespace StdLib::FileInitialization
{
    void Initialize()
    {}
}