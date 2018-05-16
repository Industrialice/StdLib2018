#include <StdCoreLib.hpp>
#include "FileToCFile.hpp"
#include "Files.hpp"

#ifdef PLATFORM_WINDOWS
    #include <io.h>
    #define fopen _wfopen
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

static bool IsProcModeSet(FileProcMode procModeCombo, FileProcMode flag);
static bool IsCacheModeSet(FileCacheMode cacheModeCombo, FileCacheMode flag);

FileToCFile::~FileToCFile()
{
    this->Close();
}

FileToCFile::FileToCFile(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode, Error<> *error)
{
    auto result = this->Open(path, openMode, procMode, FileCacheMode::Default);
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

Error<> FileToCFile::Open(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode)
{
    this->Close();

    Result<bool> isFileExists = Files::IsExists(path);
    if (!isFileExists.IsOk())
    {
        return isFileExists.GetError();
    }

    bool fileExistenceResult = isFileExists.Unwrap();

    if (openMode == FileOpenMode::OpenExisting)
    {
        if (!fileExistenceResult)
        {
            return DefaultError::NotFound();
        }
    }

    if (openMode == FileOpenMode::CreateNew)
    {
        if (fileExistenceResult)
        {
            return DefaultError::AlreadyExists();
        }
    }

    if (openMode == FileOpenMode::CreateAlways)
    {
        if (!IsProcModeSet(procMode, FileProcMode::Write))
        {
            return DefaultError::InvalidArgument("FileOpenMode::CreateAlways cannot be used without FileProcMode::Write");
        }
    }

    bool isDisableCache = false;

    if (IsCacheModeSet(cacheMode, FileCacheMode::DisableSystemWriteCache))
    {
        if (IsProcModeSet(procMode, FileProcMode::Write))
        {
            isDisableCache = true;
        }
        else
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemWriteCache is used without FileProcMode::Write");
        }
    }

    if (IsCacheModeSet(cacheMode, FileCacheMode::DisableSystemReadCache))
    {
        if (IsProcModeSet(procMode, FileProcMode::Read))
        {
            isDisableCache = true;
        }
        else
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemReadCache is used without FileProcMode::Read");
        }
    }

    pathString procModeStr;

    if (IsProcModeSet(procMode, FileProcMode::WriteAppend))
    {
        procModeStr += TSTR("a");
    }
    else if (IsProcModeSet(procMode, FileProcMode::Read) && IsProcModeSet(procMode, FileProcMode::Write))
    {
        if (fileExistenceResult)
        {
            procModeStr += TSTR("r+");
        }
        else
        {
            procModeStr += TSTR("w+");
        }
    }
    else if (IsProcModeSet(procMode, FileProcMode::Read))
    {
        procModeStr += TSTR("r");
    }
    else
    {
        ASSUME(IsProcModeSet(procMode, FileProcMode::Write));
        procModeStr += TSTR("w");
    }

    procModeStr += TSTR("b");

    _file = fopen(path.PlatformPath().data(), procModeStr.c_str());

    if (!_file)
    {
        return DefaultError::UnknownError("fopen failed");
    }

    if (isDisableCache)
    {
        if (setvbuf((FILE *)_file, 0, _IONBF, 0) != 0)
        {
            fclose((FILE *)_file);
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

    if (IsProcModeSet(procMode, FileProcMode::WriteAppend))
    {
        if (fseek((FILE *)_file, 0, SEEK_END) != 0)
        {
            fclose((FILE *)_file);
            _file = nullptr;
            return DefaultError::UnknownError("fseek failed");
        }

        _offsetToStart = ftell((FILE *)_file);
        if (_offsetToStart == -1)
        {
            fclose((FILE *)_file);
            _file = nullptr;
            return DefaultError::UnknownError("ftell failed");
        }
    }

    return DefaultError::Ok();
}

void FileToCFile::Close()
{
    if (_file)
    {
        fclose((FILE *)_file);
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
    ui32 actuallyRead = fread(target, 1, len, (FILE *)_file);
    if (read) *read = actuallyRead;
    return true;
}

bool FileToCFile::Write(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ui32 actuallyWritten = fwrite(source, 1, len, (FILE *)_file);
    if (written) *written = actuallyWritten;
    return true;
}

bool FileToCFile::Flush()
{
    ASSUME(IsOpened());
    return fflush((FILE *)_file) == 0;
}

bool FileToCFile::IsBufferingSupported() const
{
    ASSUME(IsOpened());

    bool isCachingDisabled = IsCacheModeSet(_cacheMode, FileCacheMode::DisableSystemWriteCache) || IsCacheModeSet(_cacheMode, FileCacheMode::DisableSystemReadCache);

    return !isCachingDisabled;
}

bool FileToCFile::BufferSet(ui32 size, bufferType &&buffer)
{
    ASSUME(IsOpened());

    if (!this->IsBufferingSupported())
    {
        return false;
    }

    if (fflush((FILE *)_file) != 0) // TODO: do I have to do it manually?
    {
        return false;
    }
    if (setvbuf((FILE *)_file, (char *)buffer.get(), _IOFBF, size) != 0)
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

    i64 currentOffset = ftell((FILE *)_file);
    if (currentOffset == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        return currentOffset - _offsetToStart;
    }

    ASSUME(offsetMode == FileOffsetMode::FromEnd);

    if (fseek((FILE *)_file, 0, SEEK_END) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    i64 fileEnd = ftell((FILE *)_file);
    if (fileEnd == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    i64 offsetDiff = currentOffset - fileEnd;

    if (fseek((FILE *)_file, currentOffset, SEEK_SET) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    return offsetDiff;
}

Result<i64> FileToCFile::OffsetSet(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpened());

    offset += _offsetToStart;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        if (fseek((FILE *)_file, offset, SEEK_SET) == 0)
        {
            return ftell((FILE *)_file);
        }
    }

    if (offsetMode == FileOffsetMode::FromCurrent)
    {
        if (fseek((FILE *)_file, offset, SEEK_CUR) == 0)
        {
            return ftell((FILE *)_file);
        }
    }

    if (offsetMode == FileOffsetMode::FromEnd) // we need this check if an error occured in prior conditions
    {
        if (fseek((FILE *)_file, offset, SEEK_END) == 0)
        {
            return ftell((FILE *)_file);
        }
    }

    return DefaultError::UnknownError();
}

Result<ui64> FileToCFile::SizeGet() const
{
    ASSUME(IsOpened());

    i64 currentOffset = ftell((FILE *)_file);
    if (currentOffset == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (fseek((FILE *)_file, 0, SEEK_END) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    i64 endOfFile = ftell((FILE *)_file);
    if (endOfFile == -1)
    {
        return DefaultError::UnknownError("ftell failed");
    }

    if (fseek((FILE *)_file, currentOffset, SEEK_SET) != 0)
    {
        return DefaultError::UnknownError("fseek failed");
    }

    return (ui64)(endOfFile - _offsetToStart);
}

Error<> FileToCFile::SizeSet(ui64 newSize)
{
    ASSUME(IsOpened());

    newSize += _offsetToStart;

#ifdef PLATFORM_WINDOWS
    if (_chsize(_fileno((FILE *)_file), newSize) != 0) // TODO: 2GB cap this way
    {
        return DefaultError::UnknownError("_chsize failed");
    }
#else
    if (ftruncate(fileno((FILE *)_file), newSize) != 0)
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

bool IsProcModeSet(FileProcMode procModeCombo, FileProcMode flag)
{
    return procModeCombo == (procModeCombo + flag);
}

bool IsCacheModeSet(FileCacheMode cacheModeCombo, FileCacheMode flag)
{
    return cacheModeCombo == (cacheModeCombo + flag);
}