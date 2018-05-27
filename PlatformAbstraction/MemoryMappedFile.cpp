#include <StdCoreLib.hpp>
#include "MemoryMappedFile.hpp"

using namespace StdLib;

MemoryMappedFile::MemoryMappedFile(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace, Error<> *error)
{
    Error<> result = Open(file, offset, size, isCopyOnWrite, isPrecommitSpace);
    if (error) *error = result;
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&source)
{
    _memory = source._memory;
    source._memory = nullptr;
    _size = source._size;
    _isWritable = source._isWritable;
#ifdef PLATFORM_WINDOWS
    _mappingHandle = source._mappingHandle;
#endif
}

MemoryMappedFile &MemoryMappedFile::operator = (MemoryMappedFile &&source)
{
    Close();

    _memory = source._memory;
    source._memory = nullptr;
    _size = source._size;
    _isWritable = source._isWritable;
#ifdef PLATFORM_WINDOWS
    _mappingHandle = source._mappingHandle;
#endif

    return *this;
}

bool MemoryMappedFile::IsOpened() const
{
    return _memory != nullptr;
}

bool MemoryMappedFile::IsWritable() const
{
    ASSUME(IsOpened());
    return _isWritable;
}

ui8 *MemoryMappedFile::Memory()
{
    ASSUME(IsOpened());
    return (ui8 *)_memory + _offset;
}

const ui8 *MemoryMappedFile::Memory() const
{
    ASSUME(IsOpened());
    return (ui8 *)_memory + _offset;
}

const ui8 *MemoryMappedFile::CMemory() const
{
    ASSUME(IsOpened());
    return (ui8 *)_memory + _offset;
}

uiw MemoryMappedFile::Size()
{
    ASSUME(IsOpened());
    return _size;
}

MemoryStreamFixedExternal MemoryMappedFile::ToMemoryStream()
{
    ASSUME(IsOpened());
    if (_isWritable)
    {
        return {_memory, _size, _size};
    }
    return {CMemory(), _size, _size};
}

MemoryStreamFixedExternal MemoryMappedFile::ToMemoryStream() const
{
    ASSUME(IsOpened());
    return {CMemory(), _size, _size};
}

MemoryStreamFixedExternal MemoryMappedFile::ToCMemoryStream() const
{
    ASSUME(IsOpened());
    return {CMemory(), _size, _size};
}
