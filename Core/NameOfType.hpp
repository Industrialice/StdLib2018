#pragma once

#include "CoreHeader.hpp"

WARNING_PUSH
WARNING_DISABLE_LARGE_BY_VALUE_COPY

namespace StdLib
{
	template <typename T> struct __NameOfType
	{
		template <uiw Size> static constexpr uiw StrStr(const std::array<char, Size> &source, uiw offset, const char *str)
		{
			auto equals = [](const char *left, const char *right) constexpr
			{
				for (uiw index = 0; right[index]; ++index)
				{
					if (left[index] != right[index])
					{
						return false;
					}
				}
				return true;
			};

			for (uiw index = offset; index < source.size(); ++index)
			{
				if (equals(&source[index], str))
				{
					return index;
				}
			}

			return 0; // unreachable
		}

		template <bool IsGetReadable, uiw Size> static constexpr auto Convert(const std::array<char, Size> &source)
		{
			constexpr char leftName[] = "NameOfType<";
			constexpr char rightNameGet[] = ">::Get";
			constexpr char rightNameGetReadable[] = ">::GetReadable";

			constexpr uiw leftLength = CountOf(leftName) - 1;

			uiw leftIndex = StrStr(source, 0, leftName) + leftLength;
			uiw rightIndex = StrStr(source, leftIndex, IsGetReadable ? rightNameGetReadable : rightNameGet);

			if constexpr (IsGetReadable)
			{
				constexpr char structName[] = "struct ";
				constexpr char className[] = "class ";
				constexpr char anoNamespaceName[] = "`anonymous namespace'::";

				if (StrStr(source, leftIndex, structName) == leftIndex && leftIndex < rightIndex)
				{
					leftIndex += CountOf(structName) - 1;
				}
				if (StrStr(source, leftIndex, className) == leftIndex && leftIndex < rightIndex)
				{
					leftIndex += CountOf(className) - 1;
				}
				if (StrStr(source, leftIndex, anoNamespaceName) == leftIndex && leftIndex < rightIndex)
				{
					leftIndex += CountOf(anoNamespaceName) - 1;
				}
			}

			uiw length = rightIndex - leftIndex;

			std::array<char, Size> target{};

			for (uiw index = 0; index < length; ++index)
			{
				target[index] = source[leftIndex + index];
			}

			return std::make_pair(target, length + 1);
		}

		template <uiw Size> static constexpr std::array<char, Size> ArrayFromChar(const char *str)
		{
			std::array<char, Size> result{};
			for (uiw index = 0; index < Size; ++index)
			{
				result[index] = str[index];
			}
			return result;
		}

		template <uiw TargetSize, uiw SourceSize> static constexpr std::array<char, TargetSize> Strip(const std::array<char, SourceSize> &source)
		{
			std::array<char, TargetSize> target{};
			for (uiw index = 0; index < TargetSize; ++index)
			{
				target[index] = source[index];
			}
			return target;
		}

		static constexpr auto Get()
		{
			constexpr const char *cname = PRETTY_FUNCTION;

			constexpr uiw length = [](const char *name) constexpr -> uiw
			{
				uiw length = 0;
				for (; name[length]; ++length);
				return length;
			}(cname)+1;

			constexpr std::array<char, length> name = ArrayFromChar<length>(cname);

			constexpr auto converted = Convert<false>(name);

			return Strip<converted.second>(converted.first);
		}

		static constexpr auto GetReadable()
		{
			constexpr const char *cname = PRETTY_FUNCTION;

			constexpr uiw length = [](const char *name) constexpr -> uiw
			{
				uiw length = 0;
				for (; name[length]; ++length);
				return length;
			}(cname)+1;

			constexpr std::array<char, length> name = ArrayFromChar<length>(cname);

			constexpr auto converted = Convert<true>(name);

			return Strip<converted.second>(converted.first);
		}

		template <bool IsFormatForReadability> static constexpr auto GetConditional()
		{
			if constexpr (IsFormatForReadability)
			{
				return GetReadable();
			}
			else
			{
				return Get();
			}
		}
	};

	template <typename T, bool IsFormatForReadability = false> constexpr auto NameOfType = __NameOfType<T>::template GetConditional<IsFormatForReadability>();

	namespace _Private
	{
		template <uiw Size> inline constexpr bool _ArrayStringEquals(const std::array<char, Size> &left, const char *right)
		{
			for (uiw index = 0; index < Size; ++index)
			{
				if (left[index] != right[index])
				{
					return false;
				}
			}
			return true;
		}

		struct SimpleStruct
		{};

		static_assert(_ArrayStringEquals(NameOfType<int>, "int"));
		static_assert(_ArrayStringEquals(NameOfType<double>, "double"));

		#ifdef _MSC_VER
			static_assert(_ArrayStringEquals(NameOfType<SimpleStruct>, "struct StdLib::_Private::SimpleStruct"));
			static_assert(_ArrayStringEquals(NameOfType<SimpleStruct, true>, "StdLib::_Private::SimpleStruct"));
		#else
			static_assert(_ArrayStringEquals(NameOfType<SimpleStruct>, "StdLib::_Private::SimpleStruct"));
		#endif
	}
}

WARNING_POP