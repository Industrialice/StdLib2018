#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib::VirtualMemory
{
    struct PageModes
    {
        static constexpr struct PageMode : EnumCombinable<PageMode, ui32>
        {} Write = PageMode::Create(1 << 0),
            Read = PageMode::Create(1 << 1),
            Execute = PageMode::Create(1 << 2);
    };

    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Reserve(uiw size);
    Error<> Commit(void *memory, uiw size, PageModes::PageMode pageMode);
    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Alloc(uiw size, PageModes::PageMode pageMode);
    bool Free(void *memory, uiw memorySize);
    [[nodiscard]] Result<PageModes::PageMode> PageModeGet(const void *memory, uiw size); // Possible errors are InconsistentProtection on Windows, always returns Unsupported on POSIX
    Error<> PageModeSet(void *memory, uiw size, PageModes::PageMode pageMode); // can commit uncommitted memory
	[[nodiscard]] uiw PageSize();

    ERROR_CODE_DEFINITION(0, InconsistentProtection);
    [[nodiscard]] inline Error<> InconsistentProtection() { return Error<>(_ErrorCodes::InconsistentProtection(), "VirtualMemory", nullptr); }
}