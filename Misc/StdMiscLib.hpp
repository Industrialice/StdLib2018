#pragma once

#if defined(_STD_CORE_LIB_INCLUDED) && !defined(_SUPPRESS_CORE_INITIALIZE)
	#error You should not include StdCoreLib.hpp if you intend to include StdMiscLib.hpp
#endif

#if defined(_STD_PLATFORM_LIB_INCLUDED) && !defined(_SUPPRESS_PLATFORM_INITIALIZE)
	#error You should not include StdPlatformLib.hpp if you intend to include StdMiscLib.hpp
#endif

#define _SUPPRESS_PLATFORM_INITIALIZE

#include "MiscTypes.hpp"
#include "UniqueIdManager.hpp"
#include "Logger.hpp"
#include "_MiscInitialization.hpp"

#define _STD_MISC_LIB_INCLUDED