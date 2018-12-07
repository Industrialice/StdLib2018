#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    template <typename T> struct EnumCombinable
    {
    private:
        T &As()
        {
            return *((T *)this);
        }

        const T &As() const
        {
            return *((const T *)this);
        }

        const T &As(const T &other) const
        {
            return (const T &)other;
        }

    public:
        explicit operator bool() const
        {
            return As().Value() != 0;
        }

        auto AsInteger() const
        {
            static constexpr uiw sizeOf = sizeof(decltype(std::declval<T>().Value()));
            if constexpr (sizeOf == 1) return (ui8)As().Value();
            if constexpr (sizeOf == 2) return (ui16)As().Value();
            if constexpr (sizeOf == 4) return (ui32)As().Value();
            if constexpr (sizeOf == 8) return (ui64)As().Value();
        }

        bool operator == (const T &other) const
        {
            return As().Value() == As(other).Value();
        }

        bool operator != (const T &other) const
        {
            return !operator=(other);
        }

        bool operator < (const T &other) const
        {
            return As().Value() < As(other).Value();
        }

        bool operator > (const T &other) const
        {
            return As().Value() > As(other).Value();
        }

        auto Combined(const T &other) const
        {
            using finalType = decltype(std::declval<T>().Value());
            return finalType(AsInteger() | other.AsInteger());
        }
    };
}