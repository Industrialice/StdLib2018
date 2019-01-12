#pragma once

namespace StdLib::SystemInfo
{
	enum class Arch
	{
		Unwnown, x86, x64, ARM, ARM64, Itanium, MIPS64, SH3
	};

    enum class Platform
    {
        Unknown, Android, Windows, Emscripten, iOS, macOS, Linux
    };

	Arch CPUArchitecture();
	ui32 LogicalCPUCores();
	ui32 PhysicalCPUCores();
	uiw AllocationAlignment();
    uiw MemoryPageSize(); // size in bytes
    Platform CurrentPlatform();
}