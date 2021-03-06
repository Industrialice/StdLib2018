#include "_PreHeader.hpp"
#include "File.hpp"
#include "FileSystem.hpp"
#include "PlatformErrorResolve.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace StdLib;

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, std::optional<FileShareModes::FileShareMode> shareMode)
{
    Close();

    offset = std::min<ui64>(offset, i64_max);

	// TODO: why is shareMode completely ignored?
	if (!shareMode)
	{
		if (procMode.Contains(FileProcModes::Write))
		{
			shareMode = FileShareModes::None;
		}
		else
		{
			shareMode = FileShareModes::Read;
		}
	}

    int flags = 0;

    if (procMode.Contains(FileProcModes::ReadWrite))
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

		if (!procMode.Contains(FileProcModes::Write))
		{
			return DefaultError::InvalidArgument("FileProcModes::Write must be specified with OpenMode::CreateAlways and OpenMode::CreateNew modes");
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
    int hfile = open(pnn.PlatformPath().data(), flags, S_IRWXU | S_IRWXG | S_IRWXO); // TODO: file locks
    umask(processMask);
    if (hfile == -1)
    {
        return PlatformErrorResolve();
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
            return PlatformErrorResolve();
        }
        _offsetToStart = std::min<i64>(stats.st_size, offset);
        if (lseek64(hfile, _offsetToStart, SEEK_SET) == -1)
        {
            close(hfile);
            return PlatformErrorResolve();
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

Error<> File::Open(FileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

Result<FilePath> File::PNN() const
{
    ASSUME(IsOpen());
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
    if (!IsOpen())
    {
        return;
    }
    FlushWriteBuffers(false);
    if (_isOwningFileHandle)
    {
        int result = close(_handle);
        ASSUME(result == 0);
    }
    _handle = -1;
    _bufferPos = 0;
    _readBufferCurrentSize = 0;
}

Result<i64> File::Offset(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpen());

    if (!CancelCachedRead() || !FlushWriteBuffers(false)) // TODO: optimize
    {
        return DefaultError::UnknownError("Flushing buffers failed");
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
        return PlatformErrorResolve();
    }

    if (result < _offsetToStart)
    {
        result = _offsetToStart;
        result = lseek64(_handle, _offsetToStart, SEEK_SET);
        if (result == -1)
        {
            return PlatformErrorResolve();
        }
    }

    ASSUME(result >= _offsetToStart);
    return result - _offsetToStart;
}

Result<ui64> File::Size()
{
    ASSUME(IsOpen());
    if (!FlushWriteBuffers(false)) // flushing first because file pointer may not be at the end of the file, in that case we can't just return FileSize + BufferSize
    {
        return DefaultError::UnknownError("Flushing write buffers failed");
    }
    struct stat64 stats;
    if (fstat64(_handle, &stats) != 0)
    {
        return PlatformErrorResolve();
    }
    ASSUME(stats.st_size >= _offsetToStart);
    return stats.st_size - _offsetToStart;
}

Error<> File::Size(ui64 newSize)
{
    ASSUME(IsOpen());

    if (!CancelCachedRead() || !FlushWriteBuffers(false))
    {
        return DefaultError::UnknownError("Flushing buffers failed");
    }

    newSize += static_cast<ui64>(_offsetToStart);
    if (newSize < static_cast<ui64>(_offsetToStart)) // overflow
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
    ASSUME(IsOpen());
    int result = fsync(_handle);
    ASSUME(result == 0);
}

bool File::WriteToFile(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpen());
    ASSUME(source || len == 0);

#ifdef STDLIB_ENABLE_FILE_STATS
    ++_stats.writesToFileCount;
#endif

    ssize_t sswritten = write(_handle, source, len);
    if (sswritten == static_cast<ssize_t>(-1))
    {
        if (written) *written = 0;
        return false;
    }

#ifdef STDLIB_ENABLE_FILE_STATS
    _stats.bytesToFileWritten += sswritten;
#endif

    if (written) *written = static_cast<ui32>(sswritten);
    return true;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *readRet)
{
    ASSUME(IsOpen());
    ASSUME(target || len == 0);

#ifdef STDLIB_ENABLE_FILE_STATS
    ++_stats.readsFromFileCount;
#endif

    ssize_t actuallyRead = read(_handle, target, len);
    if (actuallyRead == static_cast<ssize_t>(-1))
    {
        if (readRet) *readRet = 0;
        return false;
    }

#ifdef STDLIB_ENABLE_FILE_STATS
    _stats.bytesFromFileRead += actuallyRead;
#endif

    if (readRet) *readRet = static_cast<ui32>(actuallyRead);
    return true;
}

bool File::CancelCachedRead()
{
    ASSUME(IsOpen());
    if (_bufferPos >= _readBufferCurrentSize)
    {
        ASSUME(_readBufferCurrentSize == 0 || _bufferPos == _readBufferCurrentSize);
        return true;
    }
    i32 move = static_cast<i32>(_bufferPos) - static_cast<i32>(_readBufferCurrentSize);
    off64_t result = lseek64(_handle, move, SEEK_CUR);
    _bufferPos = _readBufferCurrentSize = 0;
    return result != -1;
}

Result<i64> File::CurrentFileOffset() const
{
    off64_t offset = lseek64(_handle, 0, SEEK_CUR);
    if (offset == -1)
    {
        return PlatformErrorResolve();
    }
    ASSUME(offset >= _offsetToStart);
    return offset;
}

namespace StdLib::FileInitialization
{
    void Initialize()
    {}
}