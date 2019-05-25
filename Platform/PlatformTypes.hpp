#pragma once

#define _SUPPRESS_CORE_INITIALIZE
#include <StdCoreLib.hpp>

namespace StdLib
{
    using utf8char = char;
    using utf32char = char32_t;
    using string_view_utf8 = std::basic_string_view<utf8char>;
    using string_view_utf32 = std::basic_string_view<utf32char>;
    using string_utf8 = std::basic_string<utf8char>;
    using string_utf32 = std::basic_string<utf32char>;

#ifdef PLATFORM_WINDOWS
    using fileHandle = void *;
    static const fileHandle fileHandle_undefined = (void *)-1;
    using pathChar = wchar_t;
    constexpr bool IsPathDelim(pathChar ch)
    {
        return ch == L'\\' || ch == L'/';
    }
    constexpr bool IsValidPathDelim(pathChar ch)
    {
        return IsPathDelim(ch);
    }
#define PTHSTR "%ls"
#define TSTR(str) L##str
#else
    using fileHandle = int;
    static constexpr fileHandle fileHandle_undefined = -1;
    using pathChar = char;
    constexpr bool IsPathDelim(pathChar ch)
    {
        return ch == '\\' || ch == '/';
    }
    constexpr bool IsValidPathDelim(pathChar ch)
    {
        return ch == '/';
    }
#define PTHSTR "%s"
#define TSTR(str) str
#endif

    using pathString = std::basic_string<pathChar>;
    using pathStringView = std::basic_string_view<pathChar>;

    constexpr utf32char UTF32SentinelChar = utf32char(0xFFFFFFFF);
}