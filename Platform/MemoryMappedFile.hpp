#pragma once

#include "File.hpp"

namespace StdLib
{
    //  Note that the file must be opened as readable, writable is optional
    //  If you pass isCopyOnWrite, you can write even if the file isn't writable (the actual file will not be affected by your changes)
    //  Parameter "size" will be clamped by the size of the file
    //  You can close the file right after the mapping was created
    //  If the MemoryMappedFile is const, you can't change the mapped memory
    class MemoryMappedFile
    {
        void *_memory = nullptr;
        uiw _size;
        uiw _offset;
        bool _isWritable;
    #ifdef PLATFORM_WINDOWS
        fileHandle _mappingHandle;
    #else
        uiw _systemMappingSize;
    #endif

    public:
        ~MemoryMappedFile();
        MemoryMappedFile() = default;
        MemoryMappedFile(File &file, uiw offset = 0, uiw size = uiw_max, bool isCopyOnWrite = false, bool isPrecommitSpace = false, Error<> *error = nullptr);
        MemoryMappedFile(MemoryMappedFile &&source) noexcept;
        MemoryMappedFile &operator = (MemoryMappedFile &&source) noexcept;
        Error<> Open(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace);
        void Close();
        [[nodiscard]] bool IsOpened() const;
        void Flush() const;
        [[nodiscard]] bool IsWritable() const;
        [[nodiscard]] ui8 *Memory();
        [[nodiscard]] const ui8 *Memory() const;
        [[nodiscard]] const ui8 *CMemory() const;
        [[nodiscard]] uiw Size();
        [[nodiscard]] MemoryStreamFixedExternal ToMemoryStream();
        [[nodiscard]] MemoryStreamFixedExternal ToMemoryStream() const;
        [[nodiscard]] MemoryStreamFixedExternal ToCMemoryStream() const;
        [[nodiscard]] explicit operator bool() const;
    };
}