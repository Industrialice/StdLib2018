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

auto SystemInfo::CPUArchitecture() -> Arch
{
	#if defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
		return Arch::x86;
	#elif defined(__amd64__)
		return Arch::x64;
	#elif defined(__arm__)
		return Arch::ARM;
	#elif defined(__thumb__)
		return Arch::ARMT;
	#elif defined(__aarch64__)
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

// TODO: retrive actual physical cores count
ui32 SystemInfo::PhysicalCPUCores()
{
    ASSUME(LogicalCPUCoresValue > 0);
    return LogicalCPUCoresValue;
}

// TODO: implementation
auto SystemInfo::AcquireCacheInfo() -> std::pair<const CacheInfo *, uiw>
{
	return {nullptr, 0u};
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

// TODO: implementation
bool SystemInfo::IsDebuggerAttached()
{
	return false;
}

namespace StdLib::SystemInfo
{
	void Initialize()
	{
	    // TODO: find an adequate crossplatform solution
        AllocationAlignmentValue = 16;
	    void *addresses[16];
	    for (int i = 0; i < 16; ++i)
        {
            addresses[i] = malloc(1);
            AllocationAlignmentValue = std::min<uiw>(AllocationAlignmentValue, 1u << Funcs::IndexOfLeastSignificantNonZeroBit((uiw)addresses[i]));
        }
        for (int i = 0; i < 16; ++i)
        {
            free(addresses[i]);
        }

        LogicalCPUCoresValue = (ui32)sysconf(_SC_NPROCESSORS_CONF);
        PageSizeValue = (uiw)sysconf(_SC_PAGESIZE);
    }
}