#include "_PreHeader.hpp"
#include "VirtualMemory.hpp"
#include "PlatformErrorResolve.hpp"
#include <sys/mman.h>

using namespace StdLib;

namespace
{
	bool IsMadvFreeSupported;
#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
	uiw PageSizeValue;
#endif
}

static constexpr int PageModeToPosix(VirtualMemory::PageModes::PageMode pageMode);

void *VirtualMemory::Reserve(uiw size, bool isTopDown)
{
	ASSUME(size && Funcs::IsAligned(size, PageSize()));
	size = Funcs::AlignAs(size, PageSize());
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		return nullptr;
	}
	return result;
}

Error<> VirtualMemory::Commit(void *memory, uiw size, PageModes::PageMode pageMode)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));

    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (mprotect(memory, size, protection) != 0)
    {
		return PlatformErrorResolve("mprotect failed");
    }

	return DefaultError::Ok();
}

Error<> VirtualMemory::Decommit(void *memory, uiw size)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));

	void *result = mmap(memory, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		return PlatformErrorResolve("mmap failed");
	}
	if (result != memory)
	{
		munmap(result, size);
		return DefaultError::UnknownError("mprotect returned wrong memory region");
	}
	return DefaultError::Ok();
}

Error<> VirtualMemory::LazyDecommit(void *memory, uiw size)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
	if (madvise(memory, size, IsMadvFreeSupported ? MADV_FREE : MADV_DONTNEED) != 0)
	{
		return PlatformErrorResolve("madvise failed");
	}
	return DefaultError::Ok();
}

void *VirtualMemory::Alloc(uiw size, bool isTopDown, PageModes::PageMode pageMode)
{
	ASSUME(size && Funcs::IsAligned(size, PageSize()));
    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return nullptr;
    }
    void *result = mmap(0, size, protection, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		return nullptr;
	}
	return result;
}

bool VirtualMemory::Free(void *memory, uiw size)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
    return munmap(memory, size) == 0;
}

auto VirtualMemory::PageModeRequest(const void *memory, uiw size) -> Result<PageModes::PageMode>
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
    return DefaultError::Unsupported();
}

Error<> VirtualMemory::PageModeChange(void *memory, uiw size, PageModes::PageMode pageMode)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));

    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (mprotect(memory, size, protection) != 0)
    {
		return PlatformErrorResolve("mprotect failed");
    }

	return DefaultError::Ok();
}

bool VirtualMemory::IsOvercommitOS()
{
#ifdef PLATFORM_EMSCRIPTEN
	return false;
#else
	return true;
#endif
}

bool VirtualMemory::IsFullLazyDecommitSupported()
{
	return IsMadvFreeSupported;
}

#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
uiw VirtualMemory::PageSize()
{
	ASSUME(PageSizeValue > 0);
	return PageSizeValue;
}
#endif

constexpr int PageModeToPosix(VirtualMemory::PageModes::PageMode pageMode)
{
	using VirtualMemory::PageModes;

	if (pageMode == PageModes::ReadWrite)
	{
		return PROT_WRITE | PROT_READ;
	}
	if (pageMode == PageModes::Read)
	{
		return PROT_READ;
	}
	if (pageMode == PageModes::NoAccess)
	{
		return PROT_NONE;
	}
	if (pageMode == PageModes::Execute)
	{
		return PROT_EXEC;
	}
	if (pageMode == PageModes::Execute.Combined(PageModes::Read))
	{
		return PROT_EXEC | PROT_READ;
	}
	if (pageMode == PageModes::Execute.Combined(PageModes::Read).Combined(PageModes::Write))
	{
		return PROT_EXEC | PROT_WRITE | PROT_READ;
	}

	return -1;
}

namespace StdLib::VirtualMemory
{
    void Initialize()
    {
		#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
			int pageSize = sysconf(_SC_PAGESIZE);
			if (pageSize == -1)
			{
				SOFTBREAK;
				PageSizeValue = 4096;
			}
			else
			{
				PageSizeValue = static_cast<uiw>(pageSize);
			}
		#endif
			
		IsMadvFreeSupported = []
		{
			void *memory = mmap(0, PageSize(), PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			ASSUME(memory);
			bool IsMadvFreeSupported = madvise(memory, PageSize(), MADV_FREE) == 0;
			int result = munmap(memory, PageSize());
			ASSUME(result == 0);
			return IsMadvFreeSupported;
		} ();
	}
}