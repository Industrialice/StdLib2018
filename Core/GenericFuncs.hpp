#pragma once

namespace StdLib::Funcs
{
    template <typename T, typename R> [[nodiscard]] bool AreSharedPointersEqual(T &&left, R &&right)
    {
        return left.owner_before(right) == false && right.owner_before(left) == false;
    }

	// TODO: constexpr

	// use it to store an opaque object as one of the fundamental types without breaking the strict aliasing rules
	template <uiw FundamentalSize, typename T> [[nodiscard]] FORCEINLINE auto ToFundamental(T value)
	{
		static_assert(sizeof(T) <= 8, "there're no fundamental types with sizes above 8");
		static_assert(sizeof(T) <= FundamentalSize);
		auto reinterpreted = []()
		{
			if constexpr (FundamentalSize == 1)
			{
				return ui8();
			}
			else if constexpr (FundamentalSize == 2)
			{
				return ui16();
			}
			else if constexpr (FundamentalSize == 4)
			{
				return ui32();
			}
			else
			{
				return ui64();
			}
		} ();
		static_assert(alignof(decltype(reinterpreted)) >= alignof(decltype(value)));
		MemOps::Copy(reinterpret_cast<std::byte *>(&reinterpreted), reinterpret_cast<std::byte *>(&value), sizeof(T));
		return reinterpreted;
	}

	template <typename T> [[nodiscard]] FORCEINLINE auto ToFundamental(T value)
	{
		return ToFundamental<sizeof(T)>(value);
	}

	// use it to reinterpret bits as a different type without breaking the strict aliasing rules
	template <typename T, typename R> [[nodiscard]] FORCEINLINE T Reinterpret(R value)
	{
		static_assert(sizeof(T) == sizeof(R));
		T reinterpreted;
		MemOps::Copy(reinterpret_cast<std::byte *>(&reinterpreted), reinterpret_cast<std::byte *>(&value), sizeof(T));
		return reinterpreted;
	}

    template <typename T> [[nodiscard]] FORCEINLINE T SetBit(T value, ui32 bitNum, bool bitValue)
    {
		static_assert(sizeof(T) <= 8, "cannot operate on types larger than 8 bytes");
        ASSUME(sizeof(T) * 8 > bitNum);
		auto fundamental = ToFundamental(value);
        if constexpr (sizeof(T) > 4)
        {
			auto input = static_cast<BitTestInput64Type>(fundamental);
			auto bitNumInput = static_cast<BitTestInput64Type>(bitNum);
            if (bitValue)
            {
                _BITTESTANDSET64(&input, bitNumInput);
            }
            else
            {
                _BITTESTANDRESET64(&input, bitNumInput);
            }
			fundamental = static_cast<decltype(fundamental)>(input);
        }
        else
        {
			auto input = static_cast<BitTestInputType>(fundamental);
			auto bitNumInput = static_cast<BitTestInputType>(bitNum);
            if (bitValue)
            {
                _BITTESTANDSET32(&input, bitNumInput);
            }
            else
            {
                _BITTESTANDRESET32(&input, bitNumInput);
            }
			fundamental = static_cast<decltype(fundamental)>(input);
        }
		return Reinterpret<T>(fundamental);
    }

    template <typename T> [[nodiscard]] FORCEINLINE bool IsBitSet(T value, ui32 bitNum)
    {
		static_assert(sizeof(T) <= 8, "cannot operate on types larger than 8 bytes");
        ASSUME(sizeof(T) * 8 > bitNum);
		auto fundamental = ToFundamental(value);
        if constexpr (sizeof(T) > 4)
        {
			auto input = static_cast<BitTestInput64Type>(fundamental);
			auto bitNumInput = static_cast<BitTestInput64Type>(bitNum);
            return _BITTEST64(input, bitNumInput);
        }
        else
        {
			auto input = static_cast<BitTestInputType>(fundamental);
			auto bitNumInput = static_cast<BitTestInputType>(bitNum);
            return _BITTEST32(input, bitNumInput);
        }
    }

    template <typename T, typename R = T> [[nodiscard]] FORCEINLINE constexpr R BitPos(T pos)
    {
        return static_cast<R>(1) << pos;
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfMostSignificantNonZeroBit(T value)
    {
		static_assert(sizeof(T) <= 8, "cannot operate on types larger than 8 bytes");
        ASSUME(value != 0);
		auto fundamental = ToFundamental(value);
        _IndexOfSignificantBitResultType result;
        if constexpr (sizeof(T) > 4)
        {
			auto input = static_cast<_IndexOfSignificantBitInput64Type>(fundamental);
            _MSNZB64(input, &result);
        }
        else
        {
			auto input = static_cast<_IndexOfSignificantBitInputType>(fundamental);
            _MSNZB32(input, &result);
        }
        return static_cast<ui32>(result);
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfLeastSignificantNonZeroBit(T value)
    {
		static_assert(sizeof(T) <= 8, "cannot operate on types larger than 8 bytes");
        ASSUME(value != 0);
		auto fundamental = ToFundamental(value);
		_IndexOfSignificantBitResultType result;
		if constexpr (sizeof(T) > 4)
		{
			auto input = static_cast<_IndexOfSignificantBitInput64Type>(fundamental);
			_LSNZB64(input, &result);
		}
		else
		{
			auto input = static_cast<_IndexOfSignificantBitInputType>(fundamental);
			_LSNZB32(input, &result);
		}
		return static_cast<ui32>(result);
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfMostSignificantZeroBit(T value)
    {
        return IndexOfMostSignificantNonZeroBit(static_cast<T>(~value));
    }

    template <typename T> [[nodiscard]] FORCEINLINE ui32 IndexOfLeastSignificantZeroBit(T value)
    {
        return IndexOfLeastSignificantNonZeroBit(static_cast<T>(~value));
    }

    template <typename T> [[nodiscard]] T ChangeEndianness(T value)
    {
		auto fundamental = ToFundamental(value);
        if constexpr (sizeof(value) == 8)
        {
			auto input = static_cast<_ByteSwapInput64>(fundamental);
			input = _BYTESWAP64(input);
            return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 4)
        {
			auto input = static_cast<_ByteSwapInput32>(fundamental);
			input = _BYTESWAP32(input);
			return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 2)
        {
			auto input = static_cast<_ByteSwapInput16>(fundamental);
			input = _BYTESWAP16(input);
			return Reinterpret<T>(input);
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
        ASSUME(shift < sizeof(T) * 8);
		auto fundamental = ToFundamental(value);
        if constexpr (sizeof(value) == 8)
        {
			auto input = static_cast<_RotateBitsInput64>(fundamental);
			input = _ROTATE64L(input, shift);
			return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 4)
        {
			auto input = static_cast<_RotateBitsInput32>(fundamental);
			input = _ROTATE32L(input, shift);
			return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 2)
        {
			auto input = static_cast<_RotateBitsInput16>(fundamental);
			input = _ROTATE16L(input, shift);
			return Reinterpret<T>(input);
        }
		else if constexpr (sizeof(value) == 1)
		{
			auto input = static_cast<_RotateBitsInput8>(fundamental);
			input = _ROTATE8L(input, shift);
			return Reinterpret<T>(input);
		}
		else
		{
			static_assert(false_v<T>, "incorrect size of the input value in RotateBitsLeft");
		}
    }

    template <typename T> [[nodiscard]] T RotateBitsRight(T value, ui32 shift)
    {
        ASSUME(shift < sizeof(T) * 8);
		auto fundamental = ToFundamental(value);
		if constexpr (sizeof(value) == 8)
        {
			auto input = static_cast<_RotateBitsInput64>(fundamental);
			input = _ROTATE64R(input, shift);
			return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 4)
        {
			auto input = static_cast<_RotateBitsInput32>(fundamental);
			input = _ROTATE32R(input, shift);
			return Reinterpret<T>(input);
        }
        else if constexpr (sizeof(value) == 2)
        {
			auto input = static_cast<_RotateBitsInput16>(fundamental);
			input = _ROTATE16R(input, shift);
			return Reinterpret<T>(input);
        }
		else if constexpr (sizeof(value) == 1)
		{
			auto input = static_cast<_RotateBitsInput8>(fundamental);
			input = _ROTATE8R(input, shift);
			return Reinterpret<T>(input);
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

	[[nodiscard]] inline bool IsAligned(const void *memory, uiw alignment)
	{
		ASSUME(IsPowerOf2(alignment));
		return (reinterpret_cast<uiw>(memory) & (alignment - 1)) == 0;
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