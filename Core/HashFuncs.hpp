#pragma once

#include "CoreHeader.hpp"

namespace StdLib::Hash
{
	template <typename T> [[nodiscard]] T FNVHash(const void *source, uiw length)
	{
		const ui8 *p = static_cast<const ui8 *>(source);
		if constexpr (std::is_same_v<T, ui32>)
		{
			ui32 hash = 0x811c9dc5u;
			for (uiw index = 0; index < length; ++index)
			{
				hash ^= p[index];
				hash *= 16777619u;
			}
			return hash;
		}
		else if constexpr (std::is_same_v<T, ui64>)
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

	template <typename T> [[nodiscard]] T FNVHash(std::string_view str)
	{
		return FNVHash<T>(str.data(), str.length());
	}

	template <typename T> [[nodiscard]] T FNVHash(const std::string &str)
	{
		return FNVHash<T>(str.data(), str.length());
	}

	template <typename T, typename U> [[nodiscard]] T FNVHash(const U &value)
	{
		return FNVHash<T>(&value, sizeof(U));
	}
}