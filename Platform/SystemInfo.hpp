#pragma once

namespace StdLib::SystemInfo
{
	enum class Arch
	{
		Unwnown, x86, x64, ARM, ARMT, ARM64, Emscripten
	};

    enum class Platform
    {
        Unknown, Android, Windows, Emscripten, iOS, macOS, Linux
    };

	struct CacheInfo
	{
		enum class Type
		{
			Instruction, Data, Trace, Unified
		};

		ui32 level{};
		ui32 lineSize{};
		ui32 associativity{};
		uiw size{}; // in bytes
		Type type{};
		std::optional<bool> isPerCore{}; // not available on POSIX
	};

	[[nodiscard]] Arch CPUArchitecture();
	[[nodiscard]] ui32 LogicalCPUCores();
	[[nodiscard]] ui32 PhysicalCPUCores();
	[[nodiscard]] std::pair<const CacheInfo *, uiw> AcquireCacheInfo();
	[[nodiscard]] uiw AllocationAlignment();
	[[nodiscard]] uiw MemoryPageSize(); // size in bytes
	[[nodiscard]] Platform CurrentPlatform();
	[[nodiscard]] bool IsDebuggerAttached();
}