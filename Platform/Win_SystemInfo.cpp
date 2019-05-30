#include "_PreHeader.hpp"
#include "SystemInfo.hpp"

using namespace StdLib;

namespace
{
	ui32 LogicalCPUCoresValue;
	ui32 PhysicalCPUCoresValue;
    uiw PageSizeValue;
	std::vector<SystemInfo::CacheInfo> CacheInfos;
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

auto SystemInfo::AcquireCacheInfo() -> std::pair<const CacheInfo *, uiw>
{
	return {CacheInfos.data(), CacheInfos.size()};
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

bool SystemInfo::IsDebuggerAttached()
{
	return IsDebuggerPresent();
}

namespace StdLib::SystemInfo
{
	void Initialize()
	{
		CacheInfos.clear();

		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		LogicalCPUCoresValue = sysinfo.dwNumberOfProcessors;
        PageSizeValue = sysinfo.dwPageSize;

		DWORD bufferSize = 0;

		GetLogicalProcessorInformation(0, &bufferSize);
		ASSUME(bufferSize > 0);
		ASSUME((bufferSize % sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)) == 0);
		uiw informationCount = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		auto buffer = std::make_unique<SYSTEM_LOGICAL_PROCESSOR_INFORMATION[]>(informationCount);
		BOOL result = GetLogicalProcessorInformation(buffer.get(), &bufferSize);
		ASSUME(result);

		ULONG_PTR processorMask = 0;

		for (uiw index = 0; index < informationCount; ++index)
		{
			processorMask |= buffer[index].ProcessorMask;

			if (buffer[index].Relationship == RelationCache)
			{
				auto source = buffer[index].Cache;
				CacheInfo info;

				switch (source.Type)
				{
				case CacheData:
					info.type = CacheInfo::Type::Data;
					break;
				case CacheInstruction:
					info.type = CacheInfo::Type::Instruction;
					break;
				case CacheTrace:
					info.type = CacheInfo::Type::Trace;
					break;
				case CacheUnified:
					info.type = CacheInfo::Type::Unified;
					break;
				}

				info.associativity = source.Associativity;
				info.level = source.Level;
				info.lineSize = source.LineSize;
				info.size = source.Size;
				info.isPerCore = false;

				bool isFound = false;
				for (auto &stored : CacheInfos)
				{
					if (stored.associativity == info.associativity &&
						stored.level == info.level &&
						stored.lineSize == info.lineSize &&
						stored.size == info.size &&
						stored.type == info.type)
					{
						isFound = true;
						stored.isPerCore = true;
					}
				}
				if (!isFound)
				{
					CacheInfos.push_back(info);
				}
			}
		}

		CacheInfos.shrink_to_fit();

		for (ui32 bitNumber = 0; bitNumber < sizeof(ULONG_PTR) * 8; ++bitNumber)
		{
			if (Funcs::IsBitSet(processorMask, bitNumber))
			{
				++PhysicalCPUCoresValue;
			}
		}
	}
}