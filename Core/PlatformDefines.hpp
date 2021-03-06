#pragma once

// PLATFORM_WINDOWS
// PLATFORM_ANDROID
// PLATFORM_IOS
// PLATFORM_LINUX
// PLATFORM_MACOS
// PLATFORM_EMSCRIPTEN

#include "CoreTypes.hpp"    

#if !defined(PLATFORM_WINDOWS) && (defined(_WIN32) || __has_include(<Windows.h>))
    #define PLATFORM_WINDOWS
#elif defined(__MACH__)
	#define PLATFORM_MACOS
#elif !defined(PLATFORM_EMSCRIPTEN) && __has_include(<emscripten.h>)
    #define PLATFORM_EMSCRIPTEN
#endif

#ifndef PLATFORM_WINDOWS
	#define PLATFORM_POSIX
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

#ifdef PLATFORM_WINDOWS
	#ifdef _WIN64
		static constexpr uiw MinimalGuaranteedAlignment = 16;
	#else
		static constexpr uiw MinimalGuaranteedAlignment = 8;
	#endif
#else
	static constexpr uiw MinimalGuaranteedAlignment = 8;
#endif
}