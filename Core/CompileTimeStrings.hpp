#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    namespace CompileTimeStrings
    {
        [[nodiscard]] static constexpr ui64 EncodeASCII(const char *source, uiw length, uiw start)
        {
            ui64 encoded = 0;
            if (start < length)
            {
                uiw offset = 56;
                for (uiw index = 0; index < 7; ++index, offset -= 7)
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
                for (uiw index = 0; index < 7; ++index, offset -= 7)
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
    }
}