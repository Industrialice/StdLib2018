#pragma once

#include "CoreTypes.hpp"    

#if (defined(_MSC_VER) || __has_include(<Windows.h>)) && !defined(PLATFORM_WINDOWS)
    #define PLATFORM_WINDOWS
#endif

#if __has_include(<emscripten.h>) && !defined(PLATFORM_EMSCRIPTEN)
    #define PLATFORM_EMSCRIPTEN
#endif

// TODO: check for multiple defined platforms

#ifdef PLATFORM_POSIX
    #include <signal.h>
	using errno_t = int;
#endif

namespace StdLib
{

#ifdef PLATFORM_WINDOWS
    #define _RAISE_EXCEPTION __debugbreak()
    constexpr ui32 MaxPathLength = 260;
    constexpr ui32 MaxFileNameLength = 256;
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) || defined(PLATFORM_EMSCRIPTEN)
    #define PLATFORM_POSIX
    #define _RAISE_EXCEPTION raise(SIGTRAP)
    constexpr ui32 MaxPathLength = 512;
    constexpr ui32 MaxFileNameLength = 256;
#else
    #error unknown platform
#endif
}