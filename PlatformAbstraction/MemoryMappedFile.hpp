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
        bool _isWritable;
    #ifdef PLATFORM_WINDOWS
        fileHandle _mappingHandle;
    #endif

    public:
        ~MemoryMappedFile();
        MemoryMappedFile() = default;
        MemoryMappedFile(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace, Error<> *error = nullptr);
        MemoryMappedFile(MemoryMappedFile &&source);
        MemoryMappedFile &operator = (MemoryMappedFile &&source);
        Error<> Open(File &file, uiw offset, uiw size, bool isCopyOnWrite, bool isPrecommitSpace);
        void Close();
        bool IsOpened() const;
        void Flush() const;
        bool IsWritable() const;
        ui8 *Memory();
        const ui8 *Memory() const;
        const ui8 *CMemory() const;
        uiw Size();
        MemoryStreamFixedExt ToMemoryStream();
        MemoryStreamFixedExt ToMemoryStream() const;
        MemoryStreamFixedExt ToCMemoryStream() const;
    };
}