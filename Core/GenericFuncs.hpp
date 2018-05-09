#pragma once

#include "CoreHeader.hpp"

namespace StdLib::Funcs
{
    template <typename T> void Drop(T &&) {}

    template <typename T, typename R> bool AreSharedPointersEqual(T &&left, R &&right)
    {
        return left.owner_before(right) == false && right.owner_before(left) == false;
    }

    template <typename Tin> constexpr uiw CountOf()
    {
        using T = typename std::remove_reference_t<Tin>;
        static_assert(std::is_array_v<T>, "CountOf() requires an array argument");
        static_assert(std::extent_v<T> > 0, "zero- or unknown-size array");
        return std::extent_v<T>;
    }
    #define CountOf(a) CountOf<decltype(a)>()
    #define CountOf32(a) ((ui32)CountOf<decltype(a)>())

    template <ui32 size> struct _NearestInt;
    template <> struct _NearestInt<1> { using type = ui8; };
    template <> struct _NearestInt<2> { using type = ui16; };
    template <> struct _NearestInt<4> { using type = ui32; };
    template <> struct _NearestInt<8> { using type = ui64; };

    template <typename T> T SetBit(T value, ui32 bitNum, bool bitValue)
    {
        ASSUME(sizeof(T) * 8 > bitNum);
        using intType = typename _NearestInt<sizeof(T)>::type;
        intType reinterpreted = *(intType *)&value;
        if (bitValue)
        {
            reinterpreted |= 1 << bitNum;
        }
        else
        {
            reinterpreted &= ~(1 << bitNum);
        }
        return *(T *)&reinterpreted;
    }

    template <typename T> bool IsBitSet(T value, ui32 bitNum)
    {
        ASSUME(sizeof(T) * 8 > bitNum);
        using intType = typename _NearestInt<sizeof(T)>::type;
        intType reinterpreted = *(intType *)&value;
        return (reinterpreted & (1 << bitNum)) != 0;
    }

    template <typename T, typename R = T> constexpr R BitPos(T pos)
    {
        return (R)1 << pos;
    }

    template <typename T> ui32 MostSignificantNonZeroBit(T value)
    {
        static_assert(std::is_integral_v<T>, "Cannot operate on non-integral types");
        ui32 result;
        if constexpr (sizeof(T) > 4)
        {
            _MSNZB64(value, &result);
        }
        else
        {
            std::conditional_t<std::is_signed_v<T>, i32, ui32> temp = value;
            _MSNZB32(temp, &result);
        }
        return result;
    }

    template <typename T> ui32 LeastSignificantNonZeroBit(T value)
    {
        static_assert(std::is_integral_v<T>, "Cannot operate on non-integral types");
        ui32 result;
        if constexpr (sizeof(T) > 4)
        {
            _LSNZB64(value, &result);
        }
        else
        {
            std::conditional_t<std::is_signed_v<T>, i32, ui32> temp = value;
            _LSNZB32(temp, &result);
        }
        return result;
    }
}