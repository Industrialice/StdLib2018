#include "_PreHeader.hpp"
#include "SystemInfo.hpp"

using namespace StdLib;

namespace
{
    uiw AllocationAlignmentValue;
}

// TODO:
auto SystemInfo::CPUArchitecture() -> Arch
{
    return Arch::Unwnown;
}

// TODO:
ui32 SystemInfo::LogicalCPUCores()
{
    return 0;
}

// TODO:
ui32 SystemInfo::PhysicalCPUCores()
{
    return 0;
}

uiw SystemInfo::AllocationAlignment()
{
    ASSUME(AllocationAlignmentValue > 0);
    return AllocationAlignmentValue;
}

namespace StdLib::SystemInfo
{
	void Initialize()
	{
        void *addr = malloc(1);
        AllocationAlignmentValue = 1 << Funcs::IndexOfLeastSignificantNonZeroBit((uiw)addr);
        free(addr);
    }
}