#pragma once

namespace StdLib
{
    namespace CompileTimeStrings
    {
        constexpr uiw CharsPerNumber = 9;

        [[nodiscard]] static constexpr ui64 EncodeASCII(const char *source, uiw length, uiw start)
        {
            ui64 encoded = 0;
            if (start < length)
            {
                uiw offset = 56;
                for (uiw index = 0; index < 9; ++index, offset -= 7)
                {
                    char sym = source[start + index];
                    if (!sym)
                    {
                        break;
                    }
                    ASSUME((sym & 0x80) == 0);
                    encoded |= ((ui64)sym) << offset;
                }
            }
            return encoded;
        }

        template <ui64... encoded> constexpr void DecodeASCII(char *target, uiw length)
        {
            auto decodeNumber = [](ui64 number, char *target, uiw length, uiw &start) constexpr
            {
                uiw offset = 56;
                for (uiw index = 0; index < 9; ++index, offset -= 7)
                {
                    char sym = (char)((number >> offset) & 0x7F);
                    if (start < length)
                    {
                        target[start] = sym;
                    }
                    ++start;
                }
            };
            uiw start = 0;
            (decodeNumber(encoded, target, length, start), ...);
            target[length - 1] = '\0';
        }

		template <uiw Length, ui64... encoded> [[nodiscard]] constexpr std::array<char, Length> DecodeASCIIToArray()
		{
			std::array<char, Length> name{};
			DecodeASCII<encoded...>(name.data(), name.size());
			return name;
		}

        template <ui64... encoded> [[nodiscard]] constexpr uiw DecodeASCIIToLength()
        {
            auto decodeNumber = [](ui64 number, uiw &start) constexpr
            {
                uiw offset = 56;
                for (uiw index = 0; index < 9; ++index, offset -= 7)
                {
                    char sym = (char)((number >> offset) & 0x7F);
                    if (sym == 0)
                    {
                        break;
                    }
                    ++start;
                }
            };
            uiw start = 0;
            (decodeNumber(encoded, start), ...);
            return start;
        }
    }
}