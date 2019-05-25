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

	[[nodiscard]] Arch CPUArchitecture();
	[[nodiscard]] ui32 LogicalCPUCores();
	[[nodiscard]] ui32 PhysicalCPUCores();
	[[nodiscard]] uiw AllocationAlignment();
	[[nodiscard]] uiw MemoryPageSize(); // size in bytes
	[[nodiscard]] Platform CurrentPlatform();
}