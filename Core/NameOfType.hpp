#pragma once

#include "CoreHeader.hpp"

WARNING_PUSH
WARNING_DISABLE_LARGE_BY_VALUE_COPY

namespace StdLib
{
	template <typename T> struct __NameOfType
	{
		template <uiw Size> static constexpr uiw StrStr(const std::array<char, Size> &source, const char *str)
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

			for (uiw index = 0; index < source.size(); ++index)
			{
				if (equals(&source[index], str))
				{
					return index;
				}
			}

			return 0; // unreachable
		}

		template <uiw SourceSize, uiw TargetSize = 0> static constexpr uiw Convert(const std::array<char, SourceSize> & source, std::array<char, TargetSize> * target)
		{
			constexpr char leftName[] = "NameOfType<";
			constexpr char rightName[] = ">::Get";

			constexpr uiw leftLength = CountOf(leftName) - 1;

			uiw leftIndex = StrStr(source, leftName) + leftLength;
			uiw rightIndex = StrStr(source, rightName);

			uiw length = rightIndex - leftIndex;

			if (target)
			{
				for (uiw index = 0; index < length; ++index)
				{
					(*target)[index] = source[leftIndex + index];
				}
			}

			return length;
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

		static constexpr auto Get()
		{
			constexpr const char *cname = PRETTY_FUNCTION;

			constexpr uiw length = [](const char *name) constexpr->uiw
			{
				uiw length = 0;
				for (; name[length]; ++length);
				return length;
			}(cname)+1;

			constexpr std::array<char, length> name = ArrayFromChar<length>(cname);

			constexpr uiw finalLength = Convert<length, 0>(name, nullptr);

			std::array<char, finalLength + 1> finalName{};
			Convert(name, &finalName);

			return finalName;
		}
	};

	template <typename T> constexpr auto NameOfType = __NameOfType<T>::Get();

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
		#else
			static_assert(_ArrayStringEquals(NameOfType<SimpleStruct>, "StdLib::_Private::SimpleStruct"));
		#endif
	}
}

WARNING_POP