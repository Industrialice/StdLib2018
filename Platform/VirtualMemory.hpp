#pragma once

namespace StdLib::VirtualMemory
{
	/*
	Passed addresses and sizes must be divisible by PageSize
	Commit and Decommit can be called any amount of times for the same region, so you don't
	  need to check the current status of the region before calling them
	Free can be called only once, calling Free for an unreserved region is an error
	Free can be used to free the whole region at once, you can't use it to free only
	  particular pages within the region
	If IsOvercommitOS() is true, then you can use Alloc instead of Reserve, the OS
	  will commit the pages only when you actually use them. Always false on Windows and Emscripten,
	  true on other POSIX platforms.
	LazyDecommit marks the pages as not needed, the exact behavior depends on the OS.
	  On Windows it seems that the OS won't remove the pages from the process' working set,
	  but the pages won't be paged out if under pressure and their contents will be discarded
	  instead. On POSIX if MADV_FREE is supported, the kernel will postpone discarding the pages
	  until there's memory pressure, if the pages haven't been discarded and there's an access attempt,
	  the operation will just be cancelled. If only MADV_DONTNEED is supported, the kernel will remove the 
	  pages from the working set, and will repopulate the pages with zeroes next time you try to access them.
	*/

    struct PageModes
    {
        static constexpr struct PageMode : EnumCombinable<PageMode, ui32>
        {} NoAccess = PageMode::Create(1 << 0),
			Write = PageMode::Create(1 << 1),
            Read = PageMode::Create(1 << 2),
            Execute = PageMode::Create(1 << 3),
			ReadWrite = Write.Combined(Read);
    };

    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Reserve(uiw size, bool isTopDown = false);
    Error<> Commit(void *memory, uiw size, PageModes::PageMode pageMode = PageModes::Read.Combined(PageModes::Write));
	Error<> Decommit(void *memory, uiw size);
	Error<> LazyDecommit(void *memory, uiw size);
    [[nodiscard]] UNIQUEPTRRETURN ALLOCATORFUNC void *Alloc(uiw size, bool isTopDown = false, PageModes::PageMode pageMode = PageModes::Read.Combined(PageModes::Write));
    bool Free(void *memory, uiw size);
    [[nodiscard]] Result<PageModes::PageMode> PageModeRequest(const void *memory, uiw size); // Possible errors are InconsistentProtection on Windows, always returns Unsupported on POSIX
    Error<> PageModeChange(void *memory, uiw size, PageModes::PageMode pageMode); // can commit uncommitted memory
	[[nodiscard]] bool IsFullLazyDecommitSupported();

	[[nodiscard]] constexpr bool IsOvercommitOS()
	{
		#if defined(PLATFORM_EMSCRIPTEN) || defined(PLATFORM_WINDOWS)
			return false;
		#else
			return true;
		#endif
	}

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