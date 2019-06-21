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
	struct PlatformFileTime
	{
		ui32 dwLowDateTime;
		ui32 dwHighDateTime;
	};
	struct FileEnumInfo
	{
		ui32 dwFileAttributes;
		PlatformFileTime ftCreationTime;
		PlatformFileTime ftLastAccessTime;
		PlatformFileTime ftLastWriteTime;
		ui32 nFileSizeHigh;
		ui32 nFileSizeLow;
		ui32 dwReserved0;
		ui32 dwReserved1;
		wchar_t cFileName[260];
		wchar_t cAlternateFileName[14];
	};
	using ConsoleHandle = void *;
	static const ConsoleHandle ConsoleHandle_undefined = (void *)-1;
    using FileHandle = void *;
    static const FileHandle FileHandle_undefined = (void *)-1;
    using PathChar = wchar_t;
    constexpr bool IsPathDelim(PathChar ch)
    {
        return ch == L'\\' || ch == L'/';
    }
    constexpr bool IsValidPathDelim(PathChar ch)
    {
        return IsPathDelim(ch);
    }
	#define PTHSTR "%ls"
	#define TSTR(str) L##str
#else
    struct FileEnumInfo
    {
        ui64 d_ino;
        i64 d_off;
        unsigned short d_reclen;
        unsigned char d_type;
        char d_name[256];
    };
	using ConsoleHandle = int;
	static constexpr ConsoleHandle ConsoleHandle_undefined = -1;
    using FileHandle = int;
    static constexpr FileHandle FileHandle_undefined = -1;
    using PathChar = char;
    constexpr bool IsPathDelim(PathChar ch)
    {
        return ch == '\\' || ch == '/';
    }
    constexpr bool IsValidPathDelim(PathChar ch)
    {
        return ch == '/';
    }
	#define PTHSTR "%s"
	#define TSTR(str) str
#endif

    using pathString = std::basic_string<PathChar>;
    using pathStringView = std::basic_string_view<PathChar>;

    constexpr utf32char UTF32SentinelChar = utf32char(0xFFFFFFFF);
}