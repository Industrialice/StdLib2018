#include "_PreHeader.hpp"
#include "SystemInfo.hpp"

#include <thread>

using namespace StdLib;

namespace
{
	ui32 LogicalCPUCoresValue;
	ui32 PhysicalCPUCoresValue;
    uiw PageSizeValue;
}

auto SystemInfo::CPUArchitecture() -> Arch
{
	#if defined(_M_AMD64)
		return Arch::x64;
	#elif defined(_M_IX86)
		return Arch::x86;
	#elif defined(_M_ARM)
		return Arch::ARM;
	#elif defined(_M_ARMT)
		return Arch::ARMT;
	#elif defined(_M_ARM64)
		return Arch::ARM64;
	#elif defined(PLATFORM_EMSCRIPTEN)
		return Arch::Emscripten;
	#else
		return Arch::Unwnown;
	#endif
}

ui32 SystemInfo::LogicalCPUCores()
{
	ASSUME(LogicalCPUCoresValue > 0);
	return LogicalCPUCoresValue;
}

ui32 SystemInfo::PhysicalCPUCores()
{
	ASSUME(PhysicalCPUCoresValue > 0);
	return PhysicalCPUCoresValue;
}

uiw SystemInfo::AllocationAlignment()
{
	return MEMORY_ALLOCATION_ALIGNMENT;
}

uiw SystemInfo::MemoryPageSize()
{
    ASSUME(PageSizeValue > 0);
    return PageSizeValue;
}

namespace StdLib::SystemInfo
{
	void Initialize()
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		LogicalCPUCoresValue = sysinfo.dwNumberOfProcessors;
		PhysicalCPUCoresValue = LogicalCPUCoresValue; // TODO: determine this value
        PageSizeValue = sysinfo.dwPageSize;
	}
}