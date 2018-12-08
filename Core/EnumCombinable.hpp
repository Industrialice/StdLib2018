#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    template <bool hasDefaultConstructor> struct _OptionalDefaultConstructior
    {
    protected:
        constexpr _OptionalDefaultConstructior() = default;
    };

    template <> struct _OptionalDefaultConstructior<true>
    {
    public:
        constexpr _OptionalDefaultConstructior() = default;
    };

    template <typename WrapperType, typename ValueType, bool hasDefaultValue = false> class EnumCombinable : _OptionalDefaultConstructior<hasDefaultValue>
    {
        static_assert(std::is_integral_v<ValueType>, "ValueType must be integral");

        ValueType _value;

        constexpr EnumCombinable(ValueType value) : _value(value)
        {}

    public:

        constexpr EnumCombinable() = default;

        static constexpr WrapperType Create(ValueType value)
        {
            return {value};
        }

        constexpr explicit operator bool() const
        {
            return _value != 0;
        }

        constexpr bool operator == (const EnumCombinable &other) const
        {
            return _value == other._value;
        }

        constexpr bool operator != (const EnumCombinable &other) const
        {
            return !operator==(other);
        }

        constexpr bool operator < (const EnumCombinable &other) const
        {
            return _value < other._value;
        }

        constexpr bool operator <= (const EnumCombinable &other) const
        {
            return _value <= other._value;
        }

        constexpr bool operator > (const EnumCombinable &other) const
        {
            return _value > other._value;
        }

        constexpr bool operator >= (const EnumCombinable &other) const
        {
            return _value >= other._value;
        }

        constexpr WrapperType Combined(const EnumCombinable &other) const
        {
            return {(ValueType)(_value | other._value)};
        }

        constexpr WrapperType &Add(const EnumCombinable &other)
        {
            _value |= other._value;
            return (WrapperType &)*this;
        }

        constexpr WrapperType &Remove(const EnumCombinable &other)
        {
            _value &= ~other._value;
            return (WrapperType &)*this;
        }

        constexpr bool Contains(const EnumCombinable &other) const
        {
            return (_value & other._value) == other._value;
        }

        constexpr bool Intersects(const EnumCombinable &other) const
        {
            return (_value & other._value) != 0;
        }

        constexpr ValueType &AsInteger()
        {
            return _value;
        }

        constexpr const ValueType &AsInteger() const
        {
            return _value;
        }
    };
}