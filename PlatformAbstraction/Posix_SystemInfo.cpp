#include "_PreHeader.hpp"
#include "SystemInfo.hpp"
#include <unistd.h>

using namespace StdLib;

namespace
{
    uiw AllocationAlignmentValue;
    ui32 LogicalCPUCoresValue;
    uiw PageSizeValue;
}

// TODO:
auto SystemInfo::CPUArchitecture() -> Arch
{
    return Arch::Unwnown;
}

ui32 SystemInfo::LogicalCPUCores()
{
    ASSUME(LogicalCPUCoresValue > 0);
    return LogicalCPUCoresValue;
}

// TODO: retrive actual physical cores count
ui32 SystemInfo::PhysicalCPUCores()
{
    ASSUME(LogicalCPUCoresValue > 0);
    return LogicalCPUCoresValue;
}

uiw SystemInfo::AllocationAlignment()
{
    ASSUME(AllocationAlignmentValue > 0);
    return AllocationAlignmentValue;
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
	    // TODO: find an adequate crossplatform solution
        AllocationAlignmentValue = 16;
	    void *addresses[10];
	    for (int i = 0; i < 10; ++i)
        {
            addresses[i] = malloc(1);
            AllocationAlignmentValue = std::min<uiw>(AllocationAlignmentValue, 1u << Funcs::IndexOfLeastSignificantNonZeroBit((uiw)addresses[i]));
        }
        for (int i = 0; i < 10; ++i)
        {
            free(addresses[i]);
        }
        if (AllocationAlignmentValue > 16)
        {
            AllocationAlignmentValue = 16;
        }

        LogicalCPUCoresValue = (ui32)sysconf(_SC_NPROCESSORS_CONF);
        PageSizeValue = (uiw)sysconf(_SC_PAGESIZE);
    }
}