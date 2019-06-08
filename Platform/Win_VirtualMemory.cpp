#include "_PreHeader.hpp"
#include "VirtualMemory.hpp"
#include "PlatformErrorResolve.hpp"

using namespace StdLib;

namespace
{
#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
    uiw Static_PageSize{};
#endif
}

static constexpr DWORD PageModeToWinAPI(VirtualMemory::PageModes::PageMode pageMode);

void *VirtualMemory::Reserve(uiw size, bool isTopDown)
{
	ASSUME(size && Funcs::IsAligned(size, PageSize()));
	DWORD flags = MEM_RESERVE;
	if (isTopDown)
	{
		flags |= MEM_TOP_DOWN;
	}
    return VirtualAlloc(0, size, flags, PAGE_NOACCESS);
}

Error<> VirtualMemory::Commit(void *memory, uiw size, PageModes::PageMode pageMode)
{
    ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (VirtualAlloc(memory, size, MEM_COMMIT, protection))
    {
        return DefaultError::Ok();
    }
    return PlatformErrorResolve("VirtualAlloc failed");
}

Error<> VirtualMemory::Decommit(void *memory, uiw size)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
	
	if (VirtualFree(memory, size, MEM_DECOMMIT) == TRUE)
	{
		return DefaultError::Ok();
	}
	return PlatformErrorResolve("VirtualFree failed");
}

Error<> VirtualMemory::LazyDecommit(void *memory, uiw size)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
	void *result = VirtualAlloc(memory, size, MEM_RESET, PAGE_NOACCESS);
	if (result != memory)
	{
		return PlatformErrorResolve("VirtualAlloc failed");
	}
	return DefaultError::Ok();
}

void *VirtualMemory::Alloc(uiw size, bool isTopDown, PageModes::PageMode pageMode)
{
    ASSUME(size && Funcs::IsAligned(size, PageSize()));
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return nullptr;
    }
	DWORD flags = MEM_RESERVE | MEM_COMMIT;
	if (isTopDown)
	{
		flags |= MEM_TOP_DOWN;
	}
    return VirtualAlloc(0, size, flags, protection);
}

bool VirtualMemory::Free(void *memory, uiw size)
{
    ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));
    return VirtualFree(memory, 0, MEM_RELEASE) != 0;
}

auto VirtualMemory::PageModeRequest(const void *memory, uiw size) -> Result<PageModes::PageMode>
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));

	MEMORY_BASIC_INFORMATION mbi;
    SIZE_T informationSize = VirtualQuery(memory, &mbi, sizeof(mbi));
    if (!informationSize)
    {
        return PlatformErrorResolve("VirtualQuery returned 0");
    }
    if (mbi.RegionSize < size)
    {
        return InconsistentProtection();
    }

	// mbi.Protect is 0 when the memory is not committed, return NoAccess in that case
	switch (mbi.Protect)
	{
		case PAGE_READWRITE: return PageModes::ReadWrite;
		case PAGE_READONLY: return PageModes::Read;
		case 0:
		case PAGE_NOACCESS: return PageModes::NoAccess;
		case PAGE_EXECUTE: return PageModes::Execute;
		case PAGE_EXECUTE_READ: return PageModes::Execute.Combined(PageModes::Read);
		case PAGE_EXECUTE_READWRITE: return PageModes::Execute.Combined(PageModes::Read).Combined(PageModes::Write);
	}

    SOFTBREAK;
    return DefaultError::UnknownError("Encountered unexpected WinAPI protection mode");
}

Error<> VirtualMemory::PageModeChange(void *memory, uiw size, PageModes::PageMode pageMode)
{
	ASSUME(memory && Funcs::IsAligned(memory, PageSize()) && size && Funcs::IsAligned(size, PageSize()));

    DWORD oldProtect;
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (VirtualProtect(memory, size, protection, &oldProtect))
    {
        return DefaultError::Ok();
    }
    return PlatformErrorResolve("VirtualProtect failed");
}

bool VirtualMemory::IsFullLazyDecommitSupported()
{
	return false;
}

#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
uiw VirtualMemory::PageSize()
{
    ASSUME(Static_PageSize);
    return Static_PageSize;
}
#endif

constexpr DWORD PageModeToWinAPI(VirtualMemory::PageModes::PageMode pageMode)
{
	using VirtualMemory::PageModes;

	if (pageMode == PageModes::ReadWrite)
	{
		return PAGE_READWRITE;
	}
	if (pageMode == PageModes::Read)
	{
		return PAGE_READONLY;
	}
	if (pageMode == PageModes::NoAccess)
	{
		return PAGE_NOACCESS;
	}
	if (pageMode == PageModes::Execute)
	{
		return PAGE_EXECUTE;
	}
	if (pageMode == PageModes::Execute.Combined(PageModes::Read))
	{
		return PAGE_EXECUTE_READ;
	}
	if (pageMode == PageModes::Execute.Combined(PageModes::Read).Combined(PageModes::Write))
	{
		return PAGE_EXECUTE_READWRITE;
	}

	return 0;
}

namespace StdLib::VirtualMemory
{
    void Initialize()
    {
		#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			Static_PageSize = sysinfo.dwPageSize;
		#endif
    }
}