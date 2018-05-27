#include <StdCoreLib.hpp>
#include "FileToCFile.hpp"
#include "FileSystem.hpp"

#ifdef PLATFORM_WINDOWS
    #include <io.h>
    #define fread _fread_nolock
    #define fwrite _fwrite_nolock
    #define fputc _fputc_nolock
    #define fflush _fflush_nolock
    #define fclose _fclose_nolock
    #define ftell _ftelli64_nolock
    #define fseek _fseeki64_nolock
#else
    #include <unistd.h>
#endif

using namespace StdLib;

FileToCFile::~FileToCFile()
{
    this->Close();
}

FileToCFile::FileToCFile(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, uiw offset, FileCacheMode cacheMode, FileShareMode shareMode, Error<> *error)
{
    auto result = this->Open(path, openMode, procMode, offset, cacheMode, shareMode);
    if (error) *error = result;
}

FileToCFile::FileToCFile(FileToCFile &&source)
{
    this->_file = source._file;
    source._file = nullptr;
    this->_offsetToStart = source._offsetToStart;
    this->_openMode = source._openMode;
    this->_procMode = source._procMode;
}

FileToCFile &FileToCFile::operator = (FileToCFile &&source)
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

Error<> FileToCFile::Open(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, uiw offset, FileCacheMode cacheMode, FileShareMode shareMode)
{
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
        if (!(procMode && FileProcMode::Write))
        {
            return DefaultError::InvalidArgument("FileOpenMode::CreateAlways cannot be used without FileProcMode::Write");
        }
    }

    bool isDisableCache = false;

    if (cacheMode && FileCacheMode::DisableSystemWriteCache)
    {
        if (procMode && FileProcMode::Write)
        {
            isDisableCache = true;
        }
        else
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemWriteCache is used without FileProcMode::Write");
        }
    }

    if (cacheMode && FileCacheMode::DisableSystemReadCache)
    {
        if (procMode && FileProcMode::Read)
        {
            isDisableCache = true;
        }
        else
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemReadCache is used without FileProcMode::Read");
        }
    }

    const pathChar *procModeStr;

    if (offset)
    {
        procModeStr = TSTR("a+");
    }
    else if ((procMode && FileProcMode::Read) && (procMode && FileProcMode::Write))
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
    else if (procMode && FileProcMode::Read)
    {
        procModeStr = TSTR("r");
    }
    else
    {
        ASSUME(procMode && FileProcMode::Write);
        procModeStr = TSTR("w");
    }

    if (shareMode && FileShareMode::Read)
    {
        if (procMode && FileProcMode::Write)
        {
            if (!(shareMode && FileShareMode::Write))
            {
                return DefaultError::InvalidArgument("FileShareMode::Read without FileShareMode::Write is not a valid sharable option for a file that is opened for write");
            }
        }
    }

#ifdef PLATFORM_WINDOWS
    static constexpr std::array<int, 4> sharingArray
    {
        _SH_DENYRW, // FileShareMode::None
        _SH_DENYWR, // FileShareMode::Read
        _SH_DENYRD, // FileShareMode::Write
        _SH_DENYNO  // FileShareMode::Read + FileShareMode::Write
    };
    int sharingOption = sharingArray[shareMode._value & 0b11];
    wchar_t binaryProcModeStr[4];
    wcscpy(binaryProcModeStr, procModeStr);
    wcscat(binaryProcModeStr, L"b");
    _file = _wfsopen(path.PlatformPath().data(), binaryProcModeStr, sharingOption);
#else
    char binaryProcModeStr[4];
    strcpy(binaryProcModeStr, procModeStr);
    strcat(binaryProcModeStr, "b");
    _file = fopen(path.PlatformPath().data(), procModeStr); // TODO: process shareMode
#endif
    if (!_file)
    {
        return DefaultError::UnknownError("fopen failed");
    }

    if (isDisableCache)
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

        _offsetToStart = std::min<uiw>(offset, fileSize);

        if (fseek(_file, _offsetToStart, SEEK_SET) != 0)
        {
            fclose(_file);
            _file = nullptr;
            return DefaultError::UnknownError("fseek failed");
        }
    }

    return DefaultError::Ok();
}

void FileToCFile::Close()
{
    if (_file)
    {
        fclose(_file);
    }
    _file = 0;
}

bool FileToCFile::IsOpened() const
{
    return _file != 0;
}

bool FileToCFile::Read(void *target, ui32 len, ui32 *read)
{
    ASSUME(IsOpened());
    ui32 actuallyRead = (ui32)fread(target, 1, len, _file);
    if (read) *read = actuallyRead;
    return true;
}

bool FileToCFile::Write(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ui32 actuallyWritten = (ui32)fwrite(source, 1, len, _file);
    if (written) *written = actuallyWritten;
    return true;
}

bool FileToCFile::Flush()
{
    ASSUME(IsOpened());
    return fflush(_file) == 0;
}

bool FileToCFile::IsBufferingSupported() const
{
    ASSUME(IsOpened());

    bool isCachingDisabled = (_cacheMode && FileCacheMode::DisableSystemWriteCache) || (_cacheMode && FileCacheMode::DisableSystemReadCache);

    return !isCachingDisabled;
}

bool FileToCFile::BufferSet(ui32 size, bufferType &&buffer)
{
    ASSUME(IsOpened());

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

std::pair<ui32, const void *> FileToCFile::BufferGet() const
{
    ASSUME(IsOpened());
    return {_bufferSize, _customBufferPtr.get()};
}

bool FileToCFile::IsSeekSupported() const
{
    ASSUME(IsOpened());
    return true;
}

Result<i64> FileToCFile::OffsetGet(FileOffsetMode offsetMode)
{
    ASSUME(IsOpened());

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

Result<i64> FileToCFile::OffsetSet(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpened());

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

Result<ui64> FileToCFile::SizeGet()
{
    ASSUME(IsOpened());

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

    return (ui64)(endOfFile - _offsetToStart);
}

Error<> FileToCFile::SizeSet(ui64 newSize)
{
    ASSUME(IsOpened());

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

FileProcMode FileToCFile::ProcMode() const
{
    ASSUME(IsOpened());
    return _procMode;
}

FileCacheMode FileToCFile::CacheMode() const
{
    ASSUME(IsOpened());
    return FileCacheMode::Default;
}

FileOpenMode FileToCFile::OpenModeGet() const
{
    ASSUME(IsOpened());
    return _openMode;
}