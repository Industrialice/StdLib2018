#include <StdCoreLib.hpp>
#include "MemoryMappedFile.hpp"
#define NOMINMAX
#include <Windows.h>

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError();

MemoryMappedFile::~MemoryMappedFile()
{
    Close();
}

Error<> MemoryMappedFile::Open(File &file, uiw offset, uiw size, bool isCopyOnWrite)
{
    Close();

    if (!file.IsOpened())
    {
        return DefaultError::InvalidArgument("File isn't opened");
    }
    if (!(file._procMode && FileProcMode::Read))
    {
        return DefaultError::InvalidArgument("File isn't readable");
    }

    uiw fileSize;
    if (auto fileSizeResult = file.SizeGet(); fileSizeResult)
    {
        fileSize = fileSizeResult.Unwrap();
    }
    else
    {
        return fileSizeResult.GetError();
    }

    _size = std::min(size, fileSize);

    DWORD protection = (isCopyOnWrite || !(file._procMode && FileProcMode::Write)) ? PAGE_WRITECOPY : PAGE_READWRITE; // for memory mapped files PAGE_WRITECOPY is equivalent to PAGE_READONLY
    LARGE_INTEGER sizeToMap;
    sizeToMap.QuadPart = (LONGLONG)_size;
    _mappingHandle = CreateFileMappingW(file._handle, nullptr, protection, sizeToMap.HighPart, sizeToMap.LowPart, nullptr);
    if (_mappingHandle == NULL)
    {
        return StdLib_FileError();
    }

    if (file._procMode && FileProcMode::WriteAppend)
    {
        offset += file._offsetToStart;
    }
    LARGE_INTEGER fileOffset;
    fileOffset.QuadPart = (LONGLONG)offset;

    DWORD desiredAccess;
    if (isCopyOnWrite)
    {
        desiredAccess = FILE_MAP_COPY;
        _isWritable = true;
    }
    else
    {
        if (file._procMode && FileProcMode::Write)
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

    _memory = MapViewOfFile(_mappingHandle, desiredAccess, fileOffset.HighPart, fileOffset.LowPart, _size);
    if (_memory == nullptr)
    {
        BOOL result = CloseHandle(_mappingHandle);
        ASSUME(result);
        return DefaultError::UnknownError("MapViewOfFile failed");
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
    ASSUME(IsOpened());
    BOOL result = FlushViewOfFile(_memory, _size);
    ASSUME(result);
}