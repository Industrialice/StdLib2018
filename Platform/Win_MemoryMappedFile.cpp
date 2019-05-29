#include "_PreHeader.hpp"
#include "MemoryMappedFile.hpp"

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError();

MemoryMappedFile::~MemoryMappedFile()
{
    Close();
}

Error<> MemoryMappedFile::Open(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace)
{
    Close();

    if (!file.IsOpen())
    {
        return DefaultError::InvalidArgument("File isn't open");
    }
    if (!file._procMode.Contains(FileProcModes::Read))
    {
        return DefaultError::InvalidArgument("File isn't readable");
    }

    uiw fileSize;
    if (auto fileSizeResult = file.Size(); fileSizeResult)
    {
        ui64 fileSizeUnwrapped = fileSizeResult.Unwrap();
        if ((fileSizeUnwrapped + file._offsetToStart) >= uiw_max)
        {
            return DefaultError::OutOfMemory("File is too big");
        }
        fileSize = (uiw)fileSizeUnwrapped;
    }
    else
    {
        return fileSizeResult.GetError();
    }

    uiw systemMappingSize = std::min(size, fileSize);

    DWORD protection = (isCopyOnWrite || !file._procMode.Contains(FileProcModes::Write)) ? PAGE_WRITECOPY : PAGE_READWRITE; // for memory mapped files PAGE_WRITECOPY is equivalent to PAGE_READONLY
    DWORD commitMode = isPrecommitSpace ? SEC_COMMIT : SEC_RESERVE;
    LARGE_INTEGER sizeToMap;
    sizeToMap.QuadPart = (LONGLONG)systemMappingSize;
    _mappingHandle = CreateFileMappingW(file._handle, nullptr, protection | commitMode, sizeToMap.HighPart, sizeToMap.LowPart, nullptr);
    if (_mappingHandle == NULL)
    {
        return StdLib_FileError();
    }

    DWORD desiredAccess;
    if (isCopyOnWrite)
    {
        desiredAccess = FILE_MAP_COPY;
        _isWritable = true;
    }
    else
    {
        if (file._procMode.Contains(FileProcModes::Write))
        {
            desiredAccess = FILE_MAP_WRITE;
            _isWritable = true;
        }
        else
        {
            desiredAccess = FILE_MAP_READ;
            _isWritable = false;
        }
    }

    _memory = MapViewOfFile(_mappingHandle, desiredAccess, 0, 0, systemMappingSize);
    if (_memory == nullptr)
    {
        BOOL result = CloseHandle(_mappingHandle);
        ASSUME(result);
        return DefaultError::UnknownError("MapViewOfFile failed");
    }

    _offset = offset + (uiw)file._offsetToStart;
    _size = size;

    uiw accessibleFileSize = fileSize - (uiw)file._offsetToStart;
    if (_size > accessibleFileSize)
    {
        _size = accessibleFileSize;
    }

    return DefaultError::Ok();
}

void MemoryMappedFile::Close()
{
    if (_memory)
    {
        ASSUME(_mappingHandle != NULL);
        BOOL result = UnmapViewOfFile(_memory);
        ASSUME(result);
        _memory = nullptr;
        result = CloseHandle(_mappingHandle);
        ASSUME(result);
    }
}

void MemoryMappedFile::Flush() const
{
    ASSUME(IsOpen());
    BOOL result = FlushViewOfFile(_memory, _size);
    ASSUME(result);
}