#pragma once

#include "CoreHeader.hpp"

namespace StdLib::Hash
{
	enum class Precision { P32, P64 };

	template <Precision precision> [[nodiscard]] auto FNVHash(const void *source, uiw length)
	{
		const ui8 *p = static_cast<const ui8 *>(source);
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
		return FNVHashCT<precision, T, length>((T *)source);
	}

	template <Precision precision, typename T> [[nodiscard]] constexpr auto FNVHash(const T &value)
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

	ui32 CRC32(const ui8 *message); // zero terminated
	ui32 CRC32(const ui8 *message, uiw length);
}