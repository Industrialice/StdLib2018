#include "_PreHeader.hpp"
#include "StandardFile.hpp"
#include "FileSystem.hpp"

#ifdef PLATFORM_WINDOWS
    #define fread _fread_nolock
    #define fwrite _fwrite_nolock
    #define fputc _fputc_nolock
    #define fflush _fflush_nolock
    #define fclose _fclose_nolock
    #define ftell _ftelli64_nolock
    #define fseek _fseeki64_nolock
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #if (defined(PLATFORM_ANDROID) && (__ANDROID_API__ < 24))
        #define _DEFINE_CUSTOM_FSEEKO
        int Custom_fseeko(FILE *stream, off_t offset, int whence);

        #define _DEFINE_CUSTOM_FTELLO
        off_t Custom_ftello(FILE *stream);

        #define fseek Custom_fseeko
        #define ftell Custom_ftello
    #else
        #define fseek fseeko
        #define ftell ftello
    #endif
#endif

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError();

StandardFile::~StandardFile()
{
    this->Close();
}

StandardFile::StandardFile(const FilePath &path, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, FileShareModes::FileShareMode shareMode, Error<> *error)
{
    auto result = this->Open(path, openMode, procMode, offset, cacheMode, shareMode);
    if (error) *error = result;
}

StandardFile::StandardFile(StandardFile &&source) noexcept
{
    this->_file = source._file;
    source._file = nullptr;
    this->_offsetToStart = source._offsetToStart;
    this->_openMode = source._openMode;
    this->_procMode = source._procMode;
}

StandardFile &StandardFile::operator = (StandardFile &&source) noexcept
{
    ASSUME(this != &source);
    Close();
    this->_file = source._file;
    source._file = nullptr;
    this->_offsetToStart = source._offsetToStart;
    this->_openMode = source._openMode;
    this->_procMode = source._procMode;
    return *this;
}

Error<> StandardFile::Open(const FilePath &path, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, FileShareModes::FileShareMode shareMode)
{
    offset = std::min<ui64>(i64_max, offset);

    this->Close();

    bool isFileFound = false;
    if (auto result = FileSystem::Classify(path); result)
    {
        isFileFound = true;
    }

    if (openMode == FileOpenMode::OpenExisting)
    {
        if (!isFileFound)
        {
            return DefaultError::NotFound();
        }
    }

    if (openMode == FileOpenMode::CreateNew)
    {
        if (isFileFound)
        {
            return DefaultError::AlreadyExists();
        }
    }

    if (openMode == FileOpenMode::CreateAlways)
    {
        if (!procMode.Contains(FileProcModes::Write))
        {
            return DefaultError::InvalidArgument("FileOpenMode::CreateAlways cannot be used without FileProcModes::Write");
        }
    }

	bool isDisableReadCache = cacheMode.Contains(FileCacheModes::DisableSystemReadCache);
	bool isDisableWriteCache = cacheMode.Contains(FileCacheModes::DisableSystemWriteCache);
	bool isOptimizeForSequentialRead = cacheMode.Contains(FileCacheModes::LinearRead);
	bool isOptimizeForSequentialWrite = cacheMode.Contains(FileCacheModes::LinearWrite);
	bool isOptimizeForRandomRead = cacheMode.Contains(FileCacheModes::RandomRead);
	bool isOptimizeForRandomWrite = cacheMode.Contains(FileCacheModes::RandomWrite);

	ui32 cacheReadCount = isDisableReadCache + isOptimizeForSequentialRead + isOptimizeForRandomRead;
	ui32 cacheWriteCount = isDisableWriteCache + isOptimizeForSequentialWrite + isOptimizeForRandomWrite;

	if (cacheReadCount > 1 || cacheWriteCount > 1)
	{
		return DefaultError::InvalidArgument("Conflicting settings for cache have been requested");
	}

	if (cacheReadCount && !procMode.Contains(FileProcModes::Read))
	{
		return DefaultError::InvalidArgument("Cache mode contains settings for read, but FileProcModes::Read is not requested");
	}
	if (cacheWriteCount && !procMode.Contains(FileProcModes::Write))
	{
		return DefaultError::InvalidArgument("Cache mode contains settings for write, but FileProcModes::Write is not requested");
	}

    const pathChar *procModeStr;

    if (offset)
    {
        procModeStr = TSTR("a+");
    }
    else if (procMode.Contains(FileProcModes::Read) && procMode.Contains(FileProcModes::Write))
    {
        if (isFileFound)
        {
            procModeStr = TSTR("r+");
        }
        else
        {
            procModeStr = TSTR("w+");
        }
    }
    else if (procMode.Contains(FileProcModes::Read))
    {
        procModeStr = TSTR("r");
    }
    else
    {
        ASSUME(procMode.Contains(FileProcModes::Write));
        procModeStr = TSTR("w");
    }

    if (shareMode.Contains(FileShareModes::Read))
    {
        if (procMode.Contains(FileProcModes::Write))
        {
            if (!shareMode.Contains(FileShareModes::Write))
            {
                return DefaultError::InvalidArgument("FileShareModes::Read without FileShareModes::Write is not a valid sharable option for a file that is open for write");
            }
        }
    }

#ifdef PLATFORM_WINDOWS
    static constexpr std::array<int, 4> sharingArray
    {
        _SH_DENYRW, // FileShareModes::None
        _SH_DENYWR, // FileShareModes::Read
        _SH_DENYRD, // FileShareModes::Write
        _SH_DENYNO  // FileShareModes::Read + FileShareModes::Write
    };
    int sharingOption = sharingArray[shareMode.AsInteger() & 0b11];
    wchar_t binaryProcModeStr[8];
    wcscpy(binaryProcModeStr, procModeStr);
    wcscat(binaryProcModeStr, L"b");
	if (isOptimizeForSequentialRead || isOptimizeForSequentialWrite)
	{
		wcscat(binaryProcModeStr, L"S");
	}
	else if (isOptimizeForRandomRead || isOptimizeForRandomWrite)
	{
		wcscat(binaryProcModeStr, L"R");
	}
    _file = _wfsopen(path.PlatformPath().data(), binaryProcModeStr, sharingOption);
	// FileShareModes::Delete setting is ignored
#else
    char binaryProcModeStr[4];
    strcpy(binaryProcModeStr, procModeStr);
    strcat(binaryProcModeStr, "b");
    _file = fopen(path.PlatformPath().data(), procModeStr);
#endif
    if (!_file)
    {
        return DefaultError::UnknownError("fopen failed");
    }

    if (isDisableReadCache || isDisableWriteCache)
    {
        if (setvbuf(_file, 0, _IONBF, 0) != 0)
        {
            fclose(_file);
            _file = nullptr;
            return DefaultError::UnknownError("setvbuf failed, cannot disable caching");
        }
    }

    _procMode = procMode;
    _openMode = openMode;
    _cacheMode = cacheMode;
    _offsetToStart = 0;
    _bufferSize = 0;
    _customBufferPtr = 0;

    if (offset)
    {
        if (fseek(_file, 0, SEEK_END) != 0)
        {
            fclose(_file);
            _file = nullptr;
            return DefaultError::UnknownError("fseek failed");
        }

        auto fileSize = ftell(_file);
        if (fileSize == -1)
        {
            fclose(_file);
            _file = nullptr;
            return DefaultError::UnknownError("ftell failed");
        }

        _offsetToStart = std::min<i64>(offset, fileSize);

        if (fseek(_file, _offsetToStart, SEEK_SET) != 0)
        {
            fclose(_file);
            _file = nullptr;
            return DefaultError::UnknownError("fseek failed");
        }
    }

    return DefaultError::Ok();
}

FileOpenMode StandardFile::OpenMode() const
{
	ASSUME(IsOpen());
	return _openMode;
}

TypeId StandardFile::Type() const
{
	return GetTypeId();
}

void StandardFile::Close()
{
    if (_file)
    {
        fclose(_file);
    }
    _file = 0;
}

bool StandardFile::IsOpen() const
{
    return _file != 0;
}

bool StandardFile::Read(void *RSTR target, ui32 len, ui32 *RSTR read)
{
    ASSUME(IsOpen());
    ui32 actuallyRead = (ui32)fread(target, 1, len, _file);
    if (read) *read = actuallyRead;
    return true;
}

bool StandardFile::Write(const void *source, ui32 len, ui32 *RSTR written)
{
    ASSUME(IsOpen());
    ui32 actuallyWritten = (ui32)fwrite(source, 1, len, _file);
    if (written) *written = actuallyWritten;
    return true;
}

bool StandardFile::Flush()
{
    ASSUME(IsOpen());
    return fflush(_file) == 0;
}

bool StandardFile::IsBufferingSupported() const
{
    ASSUME(IsOpen());

    bool isCachingDisabled = _cacheMode.Contains(FileCacheModes::DisableSystemWriteCache) || _cacheMode.Contains(FileCacheModes::DisableSystemReadCache);

    return !isCachingDisabled;
}

bool StandardFile::Buffer(ui32 size, bufferType &&buffer)
{
    ASSUME(IsOpen());

    if (!this->IsBufferingSupported())
    {
        return false;
    }

    if (fflush(_file) != 0) // TODO: do I have to do it manually?
    {
        return false;
    }
    int mode = size > 0 ? _IOFBF : _IONBF;
    if (setvbuf(_file, (char *)buffer.get(), mode, size) != 0)
    {
        return false;
    }

    _bufferSize = size;
    _customBufferPtr = std::move(buffer);

    return true;
}

std::pair<ui32, const void *> StandardFile::Buffer() const
{
	ASSUME(IsOpen());
    return {_bufferSize, _customBufferPtr.get()};
}

bool StandardFile::IsSeekSupported() const
{
    return true;
}

Result<i64> StandardFile::Offset(FileOffsetMode offsetMode)
{
    ASSUME(IsOpen());

    if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }

    i64 currentOffset = ftell(_file);
    if (currentOffset == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        return currentOffset - _offsetToStart;
    }

    ASSUME(offsetMode == FileOffsetMode::FromEnd);

    if (fseek(_file, 0, SEEK_END) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    i64 fileEnd = ftell(_file);
    if (fileEnd == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    i64 offsetDiff = currentOffset - fileEnd;

    if (fseek(_file, currentOffset, SEEK_SET) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    return offsetDiff;
}

Result<i64> StandardFile::Offset(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpen());

    int moveMethod;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        if (offset < 0)
        {
            return DefaultError::InvalidArgument("Negative offset value cannot be used with FileOffsetMode::FromBegin");
        }
        moveMethod = SEEK_SET;
        offset += _offsetToStart;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        moveMethod = SEEK_CUR;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        if (offset > 0)
        {
            return DefaultError::InvalidArgument("Positive offset value cannot be used with FileOffsetMode::FromEnd");
        }
        moveMethod = SEEK_END;
    }

    if (fseek(_file, offset, moveMethod) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    i64 curPos = ftell(_file);
    if (curPos == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (curPos < _offsetToStart)
    {
        curPos = _offsetToStart;
        if (fseek(_file, _offsetToStart, SEEK_SET) != 0)
        {
            return DefaultError::UnknownError("fseek failed");
        }
    }

    return curPos - _offsetToStart;
}

Result<ui64> StandardFile::Size()
{
    ASSUME(IsOpen());

#ifdef PLATFORM_WINDOWS

    fflush(_file);
    i64 endOfFile = _filelengthi64(_fileno(_file));
    if (endOfFile == -1)
    {
        return DefaultError::UnknownError("_filelengthi64 failed");
    }

#elif defined(PLATFORM_POSIX)

    struct stat64 stat;
    if (fstat64(fileno(_file), &stat) != 0)
    {
        return DefaultError::UnknownError("fstat64 failed");
    }
    i64 endOfFile = stat.st_size;

#else

    i64 currentOffset = ftell(_file);
    if (currentOffset == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (fseek(_file, 0, SEEK_END) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    i64 endOfFile = ftell(_file);
    if (endOfFile == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (fseek(_file, currentOffset, SEEK_SET) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

#endif

    return (ui64)(endOfFile - _offsetToStart);
}

Error<> StandardFile::Size(ui64 newSize)
{
    ASSUME(IsOpen());

    newSize += _offsetToStart;

    fflush(_file);

#ifdef PLATFORM_WINDOWS
    if (_chsize(_fileno(_file), newSize) != 0) // TODO: 2GB cap this way
    {
        return DefaultError::UnknownError("_chsize failed");
    }
#else
    if (ftruncate(fileno(_file), newSize) != 0)
    {
        return DefaultError::UnknownError("ftrancate failed");
    }
#endif

    return DefaultError::Ok();
}

FileProcModes::FileProcMode StandardFile::ProcMode() const
{
    ASSUME(IsOpen());
    return _procMode;
}

FileCacheModes::FileCacheMode StandardFile::CacheMode() const
{
    ASSUME(IsOpen());
    return FileCacheModes::Default;
}

#ifdef _DEFINE_CUSTOM_FSEEKO
    int Custom_fseeko(FILE *stream, off_t offset, int whence)
    {
        fflush(stream);
        off_t result = lseek64(fileno(stream), offset, whence);
        return result == (off_t)-1 ? -1 : 0;
    }
#endif

#ifdef _DEFINE_CUSTOM_FTELLO
    off_t Custom_ftello(FILE *stream)
    {
        fflush(stream);
        return lseek64(fileno(stream), 0L, SEEK_CUR);
    }
#endif
