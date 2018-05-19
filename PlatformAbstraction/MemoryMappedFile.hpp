#pragma once

namespace StdLib
{
    //  Note that the file must be opened as readable, writable is optional
    //  If you pass isCopyOnWrite, you can write even if the file isn't writable (the actual file will not be affected by your changes)
    //  Pass uiw_max as size to map the whole file
    //  You can close the file right after the mapping was created
    //  If the MemoryMappedFile is const, you can't change the mapped memory
    class MemoryMappedFile
    {
        void *memory = nullptr;
        uiw size;
        bool isWritable;
    #ifdef WINDOWS
        HANDLE handle;
    #endif

    public:
        ~MemoryMappedFile();
        MemoryMappedFile() = default;
    };
}