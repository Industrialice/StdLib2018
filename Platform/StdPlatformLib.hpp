#pragma once

#if defined(_STD_CORE_LIB_INCLUDED) && !defined(_SUPPRESS_CORE_INITIALIZE)
	#error You should not include StdCoreLib.hpp if you intend to include StdPlatformLib.hpp
#endif

#define _SUPPRESS_CORE_INITIALIZE

#include "PlatformTypes.hpp"
#include "FilePath.hpp"
#include "VirtualMemory.hpp"
#include "TimeMoment.hpp"
#include "File.hpp"
#include "FileSystem.hpp"
#include "MemoryMappedFile.hpp"
#include "SystemInfo.hpp"
#include "VirtualKeys.hpp"

namespace StdLib::Initialization
{
    struct PlatformParameters : CoreParameters
    {
    };

	#ifndef _SUPPRESS_PLATFORM_INITIALIZE
		void Initialize(const PlatformParameters &parameters);
	#endif
}

#define _STD_PLATFORM_LIB_INCLUDED
