#include "_PreHeader.hpp"
#include "SystemInfo.hpp"
#include "VirtualMemory.hpp"
#include <unistd.h>
#include <sys/resource.h>

#if defined(PLATFORM_MACOS)
	#include <mach/mach.h>
#endif

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

// TODO: implementation
bool SystemInfo::IsDebuggerAttached()
{
	return false;
}

uiw SystemInfo::WorkingSet()
{
	#if defined(PLATFORM_MACOS)
		struct mach_task_basic_info info;
		mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
		if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) != KERN_SUCCESS)
		{
			SOFTBREAK;
			return 0;
		}
		return static_cast<size_t>(info.resident_size);
	#else
		long rss = 0L;
		FILE *fp = fopen("/proc/self/statm", "r");
		if (!fp)
		{
			SOFTBREAK;
			return 0;
		}
		if (fscanf(fp, "%*s%ld", &rss) != 1)
		{
			SOFTBREAK;
			fclose(fp);
			return 0;
		}
		fclose(fp);
		return static_cast<size_t>(rss) * VirtualMemory::PageSize();
	#endif
}

uiw SystemInfo::PeakWorkingSet()
{
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	#if defined(PLATFORM_MACOS)
		return static_cast<size_t>(rusage.ru_maxrss);
	#else
		return static_cast<uiw>(rusage.ru_maxrss * 1024L);
	#endif
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
            AllocationAlignmentValue = std::min<uiw>(AllocationAlignmentValue, 1u << Funcs::IndexOfLeastSignificantNonZeroBit(reinterpret_cast<uiw>(addresses[i])));
        }
        for (int i = 0; i < 16; ++i)
        {
            free(addresses[i]);
        }

		int logicalCPUCores = sysconf(_SC_NPROCESSORS_CONF);
		if (logicalCPUCores == -1)
		{
			SOFTBREAK;
			LogicalCPUCoresValue = 1;
		}
		else
		{
			LogicalCPUCoresValue = static_cast<ui32>(logicalCPUCores);
		}
    }
}