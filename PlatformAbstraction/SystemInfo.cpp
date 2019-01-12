#include "_PreHeader.hpp"
#include "SystemInfo.hpp"

using namespace StdLib;

auto SystemInfo::CurrentPlatform() -> Platform
{
#if defined(PLATFORM_WINDOWS)
    return Platform::Windows;
#elif defined(PLATFORM_ANDROID)
    return Platform::Android;
#elif defined(PLATFORM_EMSCRIPTEN)
    return Platform::Emscripten;
#elif defined(PLATFORM_IOS)
    return Platform::iOS;
#elif defined(PLATFORM_MACOS)
    return Platform::macOS;
#elif defined(PLATFORM_LINUX)
    return Platform::Linux;
#else
    return Platform::Unknown;
#endif
}
