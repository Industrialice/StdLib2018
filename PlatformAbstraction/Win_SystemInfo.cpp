#include "_PreHeader.hpp"
#include "SystemInfo.hpp"

#include <thread>

using namespace StdLib;

namespace
{
	SystemInfo::Arch ArchValue;
	ui32 LogicalCPUCoresValue;
	ui32 PhysicalCPUCoresValue;
    uiw PageSizeValue;
}

auto SystemInfo::CPUArchitecture() -> Arch
{
	ASSUME(ArchValue != Arch::Unwnown);
	return ArchValue;
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

		switch (sysinfo.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_AMD64:
			ArchValue = Arch::x64;
			break;
		case PROCESSOR_ARCHITECTURE_ARM:
			ArchValue = Arch::ARM;
			break;
		case PROCESSOR_ARCHITECTURE_ARM64:
			ArchValue = Arch::ARM64;
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			ArchValue = Arch::Itanium;
			break;
		case PROCESSOR_ARCHITECTURE_INTEL:
			ArchValue = Arch::x86;
			break;
		default:
			ArchValue = Arch::Unwnown;
		}
	}
}