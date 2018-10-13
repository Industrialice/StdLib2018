#pragma once

#if defined(_STD_CORE_LIB_INCLUDED) && !defined(_SUPPRESS_CORE_INITIALIZE)
	#error You should not include StdCoreLib.hpp if you intend to include StdMiscellaneousLib.hpp
#endif

#if defined(_STD_PLATFORM_ABSTRACTION_LIB_INCLUDED) && !defined(_SUPPRESS_PLATFORM_ABSTRACTION_INITIALIZE)
	#error You should not include StdPlatformAbstractionLib.hpp if you intend to include StdMiscellaneousLib.hpp
#endif

#define _SUPPRESS_PLATFORM_ABSTRACTION_INITIALIZE

#include <StdPlatformAbstractionLib.hpp>
#include "UniqueIdManager.hpp"

namespace StdLib::Initialization
{
    struct MiscellaneousParameters : PlatformAbstractionParameters
    {
    };

    void Initialize(const MiscellaneousParameters &parameters);
}

#define _STD_MISCELLANEOUS_LIB_INCLUDED