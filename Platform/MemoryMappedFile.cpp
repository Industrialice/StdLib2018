#include "_PreHeader.hpp"
#include "MemoryMappedFile.hpp"

using namespace StdLib;

MemoryMappedFile::MemoryMappedFile(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace, Error<> *error)
{
    Error<> result = Open(file, offset, size, isCopyOnWrite, isPrecommitSpace);
    if (error) *error = result;
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&source) noexcept
{
    _memory = source._memory;
    source._memory = nullptr;
    _size = source._size;
    _isWritable = source._isWritable;
    _offset = source._offset;
#ifdef PLATFORM_WINDOWS
    _mappingHandle = source._mappingHandle;
#else
    _systemMappingSize = source._systemMappingSize;
#endif
}

MemoryMappedFile &MemoryMappedFile::operator = (MemoryMappedFile &&source) noexcept
{
    Close();

    _memory = source._memory;
    source._memory = nullptr;
    _size = source._size;
    _isWritable = source._isWritable;
    _offset = source._offset;
#ifdef PLATFORM_WINDOWS
    _mappingHandle = source._mappingHandle;
#else
    _systemMappingSize = source._systemMappingSize;
#endif

    return *this;
}

bool MemoryMappedFile::IsOpen() const
{
    return _memory != nullptr;
}

bool MemoryMappedFile::IsWritable() const
{
    ASSUME(IsOpen());
    return _isWritable;
}

std::byte *MemoryMappedFile::Memory()
{
    ASSUME(IsOpen());
    return _memory + _offset;
}

const std::byte *MemoryMappedFile::Memory() const
{
    ASSUME(IsOpen());
    return _memory + _offset;
}

const std::byte *MemoryMappedFile::CMemory() const
{
    ASSUME(IsOpen());
    return _memory + _offset;
}

uiw MemoryMappedFile::Size()
{
    ASSUME(IsOpen());
    return _size;
}

MemoryStreamFixedExternal MemoryMappedFile::ToMemoryStream()
{
    ASSUME(IsOpen());
    if (_isWritable)
    {
        return {_memory, _size, _size};
    }
    return {CMemory(), _size, _size};
}

MemoryStreamFixedExternal MemoryMappedFile::ToMemoryStream() const
{
    ASSUME(IsOpen());
    return {CMemory(), _size, _size};
}

MemoryStreamFixedExternal MemoryMappedFile::ToCMemoryStream() const
{
    ASSUME(IsOpen());
    return {CMemory(), _size, _size};
}

MemoryMappedFile::operator bool() const
{
    return IsOpen();
}
