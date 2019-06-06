#pragma once

namespace StdLib::VirtualMemory
{
    struct PageModes
    {
        static constexpr struct PageMode : EnumCombinable<PageMode, ui32>
        {} Write = PageMode::Create(1 << 0),
            Read = PageMode::Create(1 << 1),
            Execute = PageMode::Create(1 << 2);
    };

    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Reserve(uiw size, bool isTopDown = false);
    Error<> Commit(void *memory, uiw size, PageModes::PageMode pageMode = PageModes::Read.Combined(PageModes::Write));
    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Alloc(uiw size, bool isTopDown = false, PageModes::PageMode pageMode = PageModes::Read.Combined(PageModes::Write));
    bool Free(void *memory, uiw memorySize);
    [[nodiscard]] Result<PageModes::PageMode> PageModeRequest(const void *memory, uiw size); // Possible errors are InconsistentProtection on Windows, always returns Unsupported on POSIX
    Error<> PageModeChange(void *memory, uiw size, PageModes::PageMode pageMode); // can commit uncommitted memory

#ifndef STDLIB_DONT_ASSUME_PAGE_SIZE
	[[nodiscard]] constexpr uiw PageSize()
	{
		return 4096;
	}
#else
	[[nodiscard]] uiw PageSize();
#endif

    ERROR_CODE_DEFINITION(0, InconsistentProtection);
    [[nodiscard]] inline Error<> InconsistentProtection() { return Error<>(_ErrorCodes::InconsistentProtection(), "VirtualMemory", nullptr); }
}

#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
	#pragma detect_mismatch("STDLIB_DONT_ASSUME_PAGE_SIZE", "1")
#else
	#pragma detect_mismatch("STDLIB_DONT_ASSUME_PAGE_SIZE", "0")
#endif