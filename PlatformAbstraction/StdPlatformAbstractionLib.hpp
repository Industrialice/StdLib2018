#pragma once

#if defined(_STD_CORE_LIB_INCLUDED) && !defined(_SUPPRESS_CORE_INITIALIZE)
	#error You should not include StdCoreLib.hpp if you intend to include StdPlatformAbstractionLib.hpp
#endif

#define _SUPPRESS_CORE_INITIALIZE

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
    struct PlatformAbstractionParameters : CoreParameters
    {
    };

#ifndef _SUPPRESS_PLATFORM_ABSTRACTION_INITIALIZE
    void Initialize(const PlatformAbstractionParameters &parameters);
#endif
}

#define _STD_PLATFORM_ABSTRACTION_LIB_INCLUDED