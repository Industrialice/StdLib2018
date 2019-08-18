#pragma once

#include "GenericFuncs.hpp"

namespace StdLib::Hash
{
	enum class Precision { P32, P64 };

	template <Precision precision, bool IsLengthDefined> [[nodiscard]] auto _FNVHash(const void *source, uiw length)
	{
		const ui8 *p = static_cast<const ui8 *>(source);

		auto calculate = [p](auto initialConstant, auto multiplier, bool isLengthDefined, uiw length) -> decltype(initialConstant)
		{
			auto hash = initialConstant;
			for (uiw index = 0; ; ++index)
			{
				if (isLengthDefined)
				{
					if (index >= length)
					{
						break;
					}
				}
				else
				{
					if (p[index] == 0)
					{
						break;
					}
				}

				hash ^= p[index];
				hash *= multiplier;
			}
			return hash;
		};

		if constexpr (precision == Precision::P32)
		{
			return calculate(0x811c9dc5u, 16777619u, IsLengthDefined, length);
		}
		else if constexpr (precision == Precision::P64)
		{
			return calculate(0xcbf29ce484222325ull, 1099511628211ull, IsLengthDefined, length);
		}
	}

	template <Precision precision> [[nodiscard]] auto FNVHash(const char *source)
	{
		return _FNVHash<precision, false>(source, 0);
	}

	template <Precision precision> [[nodiscard]] auto FNVHash(const void *source, uiw length)
	{
		return _FNVHash<precision, true>(source, length);
	}

	template <Precision precision> [[nodiscard]] auto FNVHash(std::string_view str)
	{
		return FNVHash<precision>(str.data(), str.length());
	}

	template <Precision precision> [[nodiscard]] auto FNVHash(const std::string &str)
	{
		return FNVHash<precision>(str.data(), str.length());
	}

	// will generate different hashes compared to FNVHash if sizeof(T) is not 1
	template <Precision precision, typename T, uiw length> [[nodiscard]] constexpr auto FNVHashCT(const T *source)
	{
		const T *p = source;
		if constexpr (precision == Precision::P32)
		{
			ui32 hash = 0x811c9dc5u;
			for (uiw index = 0; index < length; ++index)
			{
				hash ^= p[index];
				hash *= 16777619u;
			}
			return hash;
		}
		else if constexpr (precision == Precision::P64)
		{
			ui64 hash = 0xcbf29ce484222325ull;
			for (uiw index = 0; index < length; ++index)
			{
				hash ^= p[index];
				hash *= 1099511628211ull;
			}
			return hash;
		}
	}

	// will generate different hashes compared to FNVHash if sizeof(T) is not 1
	template <Precision precision, typename T, uiw length, bool useReference = true> [[nodiscard]] constexpr auto FNVHashCT(const T (&source)[length])
	{
		return FNVHashCT<precision, T, length>(static_cast<const T *>(source));
	}

	template <Precision precision, typename T, typename = std::enable_if_t<std::is_pointer_v<T> == false>> [[nodiscard]] constexpr auto FNVHash(const T &value)
	{
		if constexpr (std::is_integral_v<T>)
		{
			const T arr[1] = {value};
			return FNVHashCT<precision>(arr);
		}
		else
		{
			return FNVHash<precision>(&value, sizeof(T));
		}
	}

	[[nodiscard]] ui32 CRC32(const ui8 *message); // zero terminated
	[[nodiscard]] ui32 CRC32(const ui8 *message, uiw length);

    // based on Thomas Mueller's answer from 
    // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    template <Precision precision, typename T> [[nodiscard]] auto Integer(T y)
    {
        static_assert(std::is_trivial_v<T>);
		auto fundamental = Funcs::ToFundamental(y);
        if constexpr (precision == Precision::P64)
        {
			auto x = static_cast<ui64>(fundamental);
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
            x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
            x = x ^ (x >> 31);
			return x;
        }
		else
		{
			auto x = static_cast<ui32>(fundamental);
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = (x >> 16) ^ x;
			return x;
		}
    }

    // returns the original value passed into Hash::Integer
    template <Precision precision, typename T> [[nodiscard]] auto IntegerInverse(T y)
    {
        static_assert(std::is_trivial_v<T>);
		auto fundamental = Funcs::ToFundamental(y);
		if constexpr (precision == Precision::P64)
        {
			auto x = static_cast<ui64>(fundamental);
			x = (x ^ (x >> 31) ^ (x >> 62)) * 0x319642b2d24d8ec3ULL;
            x = (x ^ (x >> 27) ^ (x >> 54)) * 0x96de1b173f119089ULL;
            x = x ^ (x >> 30) ^ (x >> 60);
			return x;
		}
		else
		{
			auto x = static_cast<ui32>(fundamental);
			x = ((x >> 16) ^ x) * 0x119de1f3;
			x = ((x >> 16) ^ x) * 0x119de1f3;
			x = (x >> 16) ^ x;
			return x;
		}
    }
}