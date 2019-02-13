#pragma once

#include "CoreHeader.hpp"

namespace StdLib::Funcs
{
    template <typename T, typename R> [[nodiscard]] bool AreSharedPointersEqual(T &&left, R &&right)
    {
        return left.owner_before(right) == false && right.owner_before(left) == false;
    }

    template <typename Tin> [[nodiscard]] constexpr uiw _CountOf()
    {
        using T = typename std::remove_reference_t<Tin>;
        static_assert(std::is_array_v<T>, "CountOf() requires an array argument");
        static_assert(std::extent_v<T> > 0, "zero- or unknown-size array");
        return std::extent_v<T>;
    }

    template <ui32 size> struct _NearestInt;
    template <> struct _NearestInt<1> { using type = ui8; };
    template <> struct _NearestInt<2> { using type = ui16; };
    template <> struct _NearestInt<4> { using type = ui32; };
    template <> struct _NearestInt<8> { using type = ui64; };

    template <typename T> [[nodiscard]] FORCEINLINE T SetBit(T value, ui32 bitNum, bool bitValue)
    {
        ASSUME(sizeof(T) * 8 > bitNum);
        using intType = typename _NearestInt<sizeof(T)>::type;
        intType reinterpreted = *(intType *)&value;
        if constexpr (sizeof(T) > 4)
        {
            if (bitValue)
            {
                _BITTESTANDSET64(&reinterpreted, bitNum);
            }
            else
            {
                _BITTESTANDRESET64(&reinterpreted, bitNum);
            }
        }
        else
        {
            ui32 widened = reinterpreted;
            if (bitValue)
            {
                _BITTESTANDSET32(&widened, bitNum);
            }
            else
            {
                _BITTESTANDRESET32(&widened, bitNum);
            }
            reinterpreted = (intType)widened;
        }
        return *(T *)&reinterpreted;
    }

    template <typename T> [[nodiscard]] FORCEINLINE bool IsBitSet(T value, ui32 bitNum)
    {
        ASSUME(sizeof(T) * 8 > bitNum);
        using intType = typename _NearestInt<sizeof(T)>::type;
        intType reinterpreted = *(intType *)&value;
        if constexpr (sizeof(T) > 4)
        {
            return _BITTEST64(&reinterpreted, bitNum);
        }
        else
        {
            ui32 widened = reinterpreted;
            return _BITTEST32(&widened, bitNum);
        }
    }

    template <typename T, typename R = T> [[nodiscard]] constexpr R BitPos(T pos)
    {
        return (R)1 << pos;
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfMostSignificantNonZeroBit(T value)
    {
        ASSUME(value != 0);
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "Cannot operate on non-integral types");
        ui32 result;
        if constexpr (sizeof(T) > 4)
        {
            _MSNZB64(value, &result);
        }
        else
        {
            using tempValType = typename _NearestInt<sizeof(T)>::type;
            tempValType temp = *(tempValType *)&value;
            ui32 temp32Bit = temp;
            _MSNZB32(temp32Bit, &result);
        }
        return result;
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfLeastSignificantNonZeroBit(T value)
    {
        ASSUME(value != 0);
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "Cannot operate on non-integral types");
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

    // TODO: IndexOfMostSignificantZeroBit and IndexOfLeastSignificantZeroBit

    template <typename T> [[nodiscard]] T ChangeEndianness(T value)
    {
        static_assert(std::is_pod_v<T>, "val is not a POD type in ChangeEndianness");
        static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "incorrect size of value in ChangeEndianness");
        if constexpr (sizeof(value) == 8)
        {
            ui64 temp = *(ui64 *)&value;
            temp = _BYTESWAP64(temp);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 4)
        {
            ui32 temp = *(ui32 *)&value;
            temp = _BYTESWAP32(temp);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 2)
        {
            ui16 temp = *(ui16 *)&value;
            temp = _BYTESWAP16(temp);
            return *(T *)&temp;
        }
        return value;
    }

    template <typename T> [[nodiscard]] T NativeToLittleEndian(T value)
    {
    #ifdef BIG_ENDIAN
        return ChangeEndianness(value);
    #endif
        return value;
    }

    template <typename T> [[nodiscard]] T NativeToBigEndian(T value)
    {
    #ifdef BIG_ENDIAN
        return value;
    #endif
        return ChangeEndianness(value);
    }

    template <typename T> [[nodiscard]] T LittleEndianToNative(T value)
    {
    #ifdef BIG_ENDIAN
        return ChangeEndianness(value);
    #endif
        return value;
    }

    template <typename T> [[nodiscard]] T BigEndianToNative(T value)
    {
    #ifdef BIG_ENDIAN
        return value;
    #endif
        return ChangeEndianness(value);
    }

    template <typename T> [[nodiscard]] T RotateBitsLeft(T value, ui32 shift)
    {
        static_assert(std::is_pod_v<T>, "val is not a POD type in RotateBitsLeft");
        static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "incorrect size of value in RotateBitsLeft");
        ASSUME(shift < sizeof(T) * 8);
        if constexpr (sizeof(value) == 8)
        {
            alignas(T) ui64 temp = *(ui64 *)&value;
            temp = _ROTATE64L(temp, shift);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 4)
        {
            alignas(T) ui32 temp = *(ui32 *)&value;
            temp = _ROTATE32L(temp, shift);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 2)
        {
            alignas(T) ui16 temp = *(ui16 *)&value;
            temp = _ROTATE16L(temp, shift);
            return *(T *)&temp;
        }
        alignas(T) ui8 temp = *(ui8 *)&value;
        temp = _ROTATE8L(temp, shift);
        return *(T *)&temp;
    }

    template <typename T> [[nodiscard]] T RotateBitsRight(T value, ui32 shift)
    {
        static_assert(std::is_pod_v<T>, "val is not a POD type in RotateBitsRight");
        static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "incorrect size of value in RotateBitsRight");
        ASSUME(shift < sizeof(T) * 8);
        if constexpr (sizeof(value) == 8)
        {
            alignas(T) ui64 temp = *(ui64 *)&value;
            temp = _ROTATE64R(temp, shift);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 4)
        {
            alignas(T) ui32 temp = *(ui32 *)&value;
            temp = _ROTATE32R(temp, shift);
            return *(T *)&temp;
        }
        if constexpr (sizeof(value) == 2)
        {
            alignas(T) ui16 temp = *(ui16 *)&value;
            temp = _ROTATE16R(temp, shift);
            return *(T *)&temp;
        }
        alignas(T) ui8 temp = *(ui8 *)&value;
        temp = _ROTATE8R(temp, shift);
        return *(T *)&temp;
    }

    template <typename T>[[nodiscard]] std::shared_ptr<typename T::element_type> SharedPtrFromUniquePtr(T &&unique)
    {
        return std::shared_ptr<typename T::element_type>(unique.release(), unique.get_deleter());
    }

    template <typename T> [[nodiscard]] std::shared_ptr<typename T::element_type> MakeSharedFromUniquePtr(T &&unique)
    {
        using type = typename T::element_type;
        static_assert(std::is_same_v<std::remove_reference_t<decltype(unique.get_deleter())>, std::default_delete<type>>, "make_shared cannot be used with custom deleters");
        if constexpr(std::is_move_constructible_v<type>)
        {
            auto shared = std::make_shared<type>(std::move(*unique));
            unique.reset();
            return shared;
        }
        else
        {
            auto shared = std::make_shared<type>(*unique);
            unique.reset();
            return shared;
        }
    }

	template <typename T> [[nodiscard]] constexpr bool IsPowerOf2(T value)
	{
		if (value <= 0)
		{
			return false;
		}
		return (value & (value - 1)) == 0;
	}

    template <typename T> void Reinitialize(T &target)
    {
        target.~T();
        new (&target) T();
    }

    template <typename T, typename U> void Reinitialize(T &target, U &&newValue)
    {
        target.~T();
        new (&target) T(std::forward<U>(newValue));
    }

    template <typename T> void Drop(T &&target)
    {
        T _ = std::move(target);
        (void)_;
    }

    template <typename T, uiw size> [[nodiscard]] constexpr std::array<T, size> SortCompileTime(const std::array<T, size> &source)
    {
        std::array<T, size> output = source;
        if (size > 0)
        {
            for (uiw i = 0; i < size - 1; ++i)
            {
                for (uiw j = i + 1; j < size; ++j)
                {
                    if (output[j] < output[i])
                    {
                        T temp = output[j];
                        output[j] = output[i];
                        output[i] = temp;
                    }
                }
            }
        }
        return output;
    }
}

#define CountOf(a) Funcs::_CountOf<decltype(a)>()

namespace StdLib::MemOps
{
    template <typename T> T *Copy(T *RSTR destination, const T *RSTR source, uiw count)
    {
        ASSUME(destination && source);
        return (T *)memcpy(destination, source, count * sizeof(T));
    }

    template <typename T> bool CopyChecked(T *RSTR destination, uiw destinationCapacity, const T *RSTR source, uiw count)
    {
        return 0 == memcpy_s(destination, destinationCapacity * sizeof(T), source, count * sizeof(T));
    }

    template <typename T> T *Move(T *destination, const T *source, uiw count)
    {
        assert(destination && source);
        return (T *)memmove(destination, source, count * sizeof(T));
    }

    template <typename T> bool MoveChecked(T *destination, uiw destinationCapacity, const T *source, uiw count)
    {
        return 0 == memmove_s(destination, destinationCapacity * sizeof(T), source, count * sizeof(T));
    }

    template <typename T> [[nodiscard]] i32 Compare(const T *target, const T *source, uiw count)
    {
        ASSUME(target && source);
        return memcmp(target, source, count * sizeof(T));
    }

    template <typename T> T *Set(T *destination, ui8 value, uiw count)
    {
        ASSUME(destination);
        return (T *)memset(destination, value, count * sizeof(T));
    }

    template <typename T> bool SetChecked(T *destination, uiw destinationCapacity, ui8 value, uiw count)
    {
        return 0 == memset_s(destination, destinationCapacity * sizeof(T), value, count * sizeof(T));
    }

    template <typename T> [[nodiscard]] T *Chr(const T *source, ui8 value, uiw count)
    {
        ASSUME(source);
        return (T *)memchr(source, value, count * sizeof(T));
    }
}