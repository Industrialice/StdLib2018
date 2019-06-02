#include "_PreHeader.hpp"
#include "MemoryMappedFile.hpp"
#include <sys/mman.h>

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
        fileSize = static_cast<uiw>(fileSizeUnwrapped);
    }
    else
    {
        return fileSizeResult.GetError();
    }

    _systemMappingSize = std::min(size, fileSize);

    int prot = PROT_READ;
    if (file._procMode.Contains(FileProcModes::Write))
    {
        prot |= PROT_WRITE;
        _isWritable = true;
    }
    else
    {
        _isWritable = false;
    }

    int flags = isCopyOnWrite ? MAP_PRIVATE : MAP_SHARED;
    flags |= isPrecommitSpace ? MAP_POPULATE : MAP_NORESERVE;

    _memory = static_cast<std::byte *>(mmap(nullptr, _systemMappingSize, prot, flags, file._handle, 0));
    if (!_memory)
    {
        return DefaultError::UnknownError("mmap failed");
    }

    _offset = offset + static_cast<uiw>(file._offsetToStart);
    _size = size;

    uiw accessibleFileSize = fileSize - static_cast<uiw>(file._offsetToStart);
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
        int result = munmap(_memory, _systemMappingSize);
        ASSUME(result == 0);
    }
}

void MemoryMappedFile::Flush() const
{
    ASSUME(IsOpen());
    int result = msync(_memory, _systemMappingSize, MS_ASYNC | MS_INVALIDATE);
    ASSUME(result == 0);
}