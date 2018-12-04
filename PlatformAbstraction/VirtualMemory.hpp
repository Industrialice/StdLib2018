#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib::VirtualMemory
{
	ENUM_COMBINABLE_WITH_OPS(PageMode, ui32,
        Write = Funcs::BitPos(0),
        Read = Funcs::BitPos(1),
        Execute = Funcs::BitPos(2));

    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Reserve(uiw size);
    Error<> Commit(void *memory, uiw size, PageMode pageMode);
    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Alloc(uiw size, PageMode pageMode);
    bool Free(void *memory, uiw memorySize);
    [[nodiscard]] Result<PageMode> PageModeGet(const void *memory, uiw size); // Possible errors are InconsistentProtection on Windows, always returns Unsupported on POSIX
    Error<> PageModeSet(void *memory, uiw size, PageMode pageMode); // can commit uncommitted memory
    uiw PageSize();

    ERROR_CODE_DEFINITION(0, InconsistentProtection);
    [[nodiscard]] inline Error<> InconsistentProtection() { return Error<>(_ErrorCodes::InconsistentProtection(), "VirtualMemory", nullptr); }
}