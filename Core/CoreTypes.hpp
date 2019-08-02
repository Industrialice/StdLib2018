#pragma once

namespace StdLib
{
    using uiw = uintptr_t;
    using iw = intptr_t;
    using ui64 = uint64_t;
    using i64 = int64_t;
    using ui32 = uint32_t;
    using i32 = int32_t;
    using ui16 = uint16_t;
    using i16 = int16_t;
    using ui8 = uint8_t;
    using i8 = int8_t;
    using f32 = float;
    using f64 = double;
    using utf8char = char;
    using utf16char = char16_t;
    using utf32char = char32_t;

    constexpr uiw uiw_max = std::numeric_limits<uiw>::max();
    constexpr uiw uiw_min = 0;
    constexpr iw iw_max = std::numeric_limits<iw>::max();
    constexpr iw iw_min = std::numeric_limits<iw>::min();
    constexpr ui64 ui64_max = 18446744073709551615ULL; // 0xFFffFFffFFffFFff
    constexpr ui64 ui64_min = 0; // 0x0000000000000000
    constexpr ui32 ui32_max = 4294967295; // 0xFFffFFff
    constexpr ui32 ui32_min = 0; // 0x00000000
    constexpr ui16 ui16_max = 65535; // 0xFFff
    constexpr ui16 ui16_min = 0; // 0x0000
    constexpr ui8 ui8_max = 255; // 0xFF
    constexpr ui8 ui8_min = 0; // 0x00
    constexpr f32 f32_max = 3.402823466e+38F;
    constexpr f32 f32_min = 1.175494351e-38F;
    constexpr f64 f64_max = 1.7976931348623158e+308;
    constexpr f64 f64_min = 2.2250738585072014e-308;
    constexpr i64 i64_max = 9223372036854775807LL; // 0x7FffFFffFFffFFff
    constexpr i64 i64_min = -9223372036854775807LL - 1; // 0x8000000000000000
    constexpr i32 i32_max = 2147483647; // 0x7FffFFff
    constexpr i32 i32_min = -2147483647 - 1; // 0x80000000
    constexpr i16 i16_max = 32767; // 0x7Fff
    constexpr i16 i16_min = -32768; // 0x8000
    constexpr i8 i8_max = 127; // 0x7F
    constexpr i8 i8_min = -128; // 0x80
    constexpr utf8char utf8char_max = std::numeric_limits<utf8char>::max();
    constexpr utf8char utf8char_min = std::numeric_limits<utf8char>::min();
    constexpr utf16char utf16char_max = std::numeric_limits<utf16char>::max();
    constexpr utf16char utf16char_min = std::numeric_limits<utf16char>::min();
    constexpr utf32char utf32char_max = std::numeric_limits<utf32char>::max();
    constexpr utf32char utf32char_min = std::numeric_limits<utf32char>::min();
}