#include <StdCoreLib.hpp>
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

    int prot = PROT_READ;
    if (file._procMode && FileProcMode::Write)
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

    _memory = mmap(nullptr, _size, prot, flags, file._handle, offset);
    if (!_memory)
    {
        return DefaultError::UnknownError("mmap failed");
    }

    return DefaultError::Ok();
}

void MemoryMappedFile::Close()
{
    if (_memory)
    {
        int result = munmap(_memory, _size);
        ASSUME(result == 0);
    }
}

void MemoryMappedFile::Flush() const
{
    ASSUME(IsOpened());
    int result = msync(_memory, _size, MS_ASYNC | MS_INVALIDATE);
    ASSUME(result == 0);
}