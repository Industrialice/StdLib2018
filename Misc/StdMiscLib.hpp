#pragma once

#if defined(_STD_CORE_LIB_INCLUDED) && !defined(_SUPPRESS_CORE_INITIALIZE)
	#error You should not include StdCoreLib.hpp if you intend to include StdMiscLib.hpp
#endif

#if defined(_STD_PLATFORM_LIB_INCLUDED) && !defined(_SUPPRESS_PLATFORM_INITIALIZE)
	#error You should not include StdPlatformLib.hpp if you intend to include StdMiscLib.hpp
#endif

#define _SUPPRESS_PLATFORM_INITIALIZE

#include <atomic>
#include <mutex>
#include <cstdarg>
#include <StdPlatformLib.hpp>
#include "UniqueIdManager.hpp"
#include "Logger.hpp"

namespace StdLib::Initialization
{
    struct MiscParameters : PlatformParameters
    {
    };

	#ifndef _SUPPRESS_MISC_INITIALIZE
		void Initialize(const MiscParameters &parameters);
	#endif
}

#define _STD_MISC_LIB_INCLUDED