#pragma once

#include "CoreHeader.hpp"

namespace StdLib::Funcs
{
    template <typename T, typename R> [[nodiscard]] bool AreSharedPointersEqual(T &&left, R &&right)
    {
        return left.owner_before(right) == false && right.owner_before(left) == false;
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

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfMostSignificantZeroBit(T value)
    {
        return IndexOfMostSignificantNonZeroBit(T(~value));
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfLeastSignificantZeroBit(T value)
    {
        return IndexOfLeastSignificantNonZeroBit(T(~value));
    }

    template <typename T> [[nodiscard]] T ChangeEndianness(T value)
    {
        static_assert(std::is_fundamental_v<T>, "input value is not a fundamental type in ChangeEndianness");
        if constexpr (sizeof(value) == 8)
        {
            ui64 temp = *(ui64 *)&value;
            temp = _BYTESWAP64(temp);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 4)
        {
            ui32 temp = *(ui32 *)&value;
            temp = _BYTESWAP32(temp);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 2)
        {
            ui16 temp = *(ui16 *)&value;
            temp = _BYTESWAP16(temp);
            return *(T *)&temp;
        }
		else if constexpr (sizeof(value) == 1)
		{
			return value;
		}
		else
		{
			static_assert(false_v<T>, "incorrect size of value in ChangeEndianness");
		}
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
        static_assert(std::is_fundamental_v<T>, "input value is not a fundamental type in RotateBitsLeft");
        ASSUME(shift < sizeof(T) * 8);
        if constexpr (sizeof(value) == 8)
        {
            alignas(T) ui64 temp = *(ui64 *)&value;
            temp = _ROTATE64L(temp, shift);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 4)
        {
            alignas(T) ui32 temp = *(ui32 *)&value;
            temp = _ROTATE32L(temp, shift);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 2)
        {
            alignas(T) ui16 temp = *(ui16 *)&value;
            temp = _ROTATE16L(temp, shift);
            return *(T *)&temp;
        }
		else if constexpr (sizeof(value) == 1)
		{
			alignas(T) ui8 temp = *(ui8 *)&value;
			temp = _ROTATE8L(temp, shift);
			return *(T *)&temp;
		}
		else
		{
			static_assert(false_v<T>, "incorrect size of the input value in RotateBitsLeft");
		}
    }

    template <typename T> [[nodiscard]] T RotateBitsRight(T value, ui32 shift)
    {
        static_assert(std::is_fundamental_v<T>, "val is not a fundamental type in RotateBitsRight");
        ASSUME(shift < sizeof(T) * 8);
        if constexpr (sizeof(value) == 8)
        {
            alignas(T) ui64 temp = *(ui64 *)&value;
            temp = _ROTATE64R(temp, shift);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 4)
        {
            alignas(T) ui32 temp = *(ui32 *)&value;
            temp = _ROTATE32R(temp, shift);
            return *(T *)&temp;
        }
        else if constexpr (sizeof(value) == 2)
        {
            alignas(T) ui16 temp = *(ui16 *)&value;
            temp = _ROTATE16R(temp, shift);
            return *(T *)&temp;
        }
		else if constexpr (sizeof(value) == 1)
		{
			alignas(T) ui8 temp = *(ui8 *)&value;
			temp = _ROTATE8R(temp, shift);
			return *(T *)&temp;
		}
		else
		{
			static_assert(false_v<T>, "incorrect size of the input value in RotateBitsRight");
		}
    }

	template <typename T> [[nodiscard]] std::shared_ptr<typename T::element_type> SharedPtrFromUniquePtr(T &&unique)
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

	template <size_t I, typename T> struct _RemoveTupleElement
	{
	};

	template <> struct _RemoveTupleElement<0, std::tuple<>>
	{
		using type = std::tuple<>;
	};

	template <typename T, typename... Ts> struct _RemoveTupleElement<0, std::tuple<T, Ts...>>
	{
		using type = std::tuple<Ts...>;
	};

	template <size_t I, typename T, typename... Ts> struct _RemoveTupleElement<I, std::tuple<T, Ts...>>
	{
		using type = decltype(std::tuple_cat(
				std::declval<std::tuple<T>>(),
				std::declval<typename _RemoveTupleElement<I - 1, std::tuple<Ts...>>::type>()));
	};

	template <size_t I, typename T> using RemoveTupleElement = typename _RemoveTupleElement<I, T>::type;

	template <typename T, typename DefaultType, uiw... Indexes> constexpr auto _TupleToArray(const T &source, std::index_sequence<Indexes...>)
	{
		if constexpr (sizeof...(Indexes) == 0)
		{
			return std::array<DefaultType, 0>{};
		}
		else
		{
			return std::make_array(std::get<Indexes>(source)...);
		}
	}

	template <typename T, typename DefaultType = T> [[nodiscard]] constexpr auto TupleToArray(const T &source)
	{
		return _TupleToArray<T, DefaultType>(source, std::make_index_sequence<std::tuple_size_v<T>>());
	}

	template <typename T, uiw... Indexes> constexpr auto _ArrayToTuple(const std::array<T, sizeof...(Indexes)> &source, std::index_sequence<Indexes...>)
	{
		return std::make_tuple(source[Indexes]...);
	}

	template <typename T, uiw size> [[nodiscard]] constexpr auto ArrayToTuple(const std::array<T, size> &source)
	{
		return _ArrayToTuple(source, std::make_index_sequence<size>());
	}
}