#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    template <bool hasDefaultConstructor> class _OptionalDefaultConstructior
    {
    protected:
        constexpr _OptionalDefaultConstructior() = default;

//#ifdef SPACESHIP_SUPPORTED
//	public:
//		[[nodiscard]] constexpr auto operator <=> (const _OptionalDefaultConstructior &) const = default;
//#endif
    };

    template <> class _OptionalDefaultConstructior<true>
    {
    public:
        constexpr _OptionalDefaultConstructior() = default;
//#ifdef SPACESHIP_SUPPORTED
//		[[nodiscard]] constexpr auto operator <=> (const _OptionalDefaultConstructior &) const = default;
//#endif
    };

    template <typename WrapperType, typename ValueType, bool hasDefaultValue = false> class EnumCombinable : public _OptionalDefaultConstructior<hasDefaultValue>
    {
        static_assert(std::is_integral_v<ValueType>, "ValueType must be integral");

        ValueType _value;

        constexpr EnumCombinable(ValueType value) : _value(value)
        {}

    public:

		using valueType = ValueType;

        constexpr EnumCombinable() = default;

		[[nodiscard]] static constexpr WrapperType Create(ValueType value)
        {
            return {value};
        }

        constexpr explicit operator bool() const
        {
            return _value != 0;
        }

//#ifdef SPACESHIP_SUPPORTED
//		[[nodiscard]] constexpr auto operator <=> (const EnumCombinable &other) const
//		{
//			return _value <=> other._value;
//		}
//#else
		[[nodiscard]] constexpr bool operator == (const EnumCombinable &other) const
		{
			return _value == other._value;
		}

		[[nodiscard]] constexpr bool operator != (const EnumCombinable &other) const
		{
			return !operator==(other);
		}

		[[nodiscard]] constexpr bool operator < (const EnumCombinable &other) const
		{
			return _value < other._value;
		}

		[[nodiscard]] constexpr bool operator <= (const EnumCombinable &other) const
		{
			return _value <= other._value;
		}

		[[nodiscard]] constexpr bool operator > (const EnumCombinable &other) const
		{
			return _value > other._value;
		}

		[[nodiscard]] constexpr bool operator >= (const EnumCombinable &other) const
		{
			return _value >= other._value;
		}
//#endif

		[[nodiscard]] constexpr WrapperType Combined(const EnumCombinable &other) const
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

		[[nodiscard]] constexpr bool Contains(const EnumCombinable &other) const
        {
            return (_value & other._value) == other._value;
        }

		[[nodiscard]] constexpr bool Intersects(const EnumCombinable &other) const
        {
            return (_value & other._value) != 0;
        }

		[[nodiscard]] constexpr ValueType &AsInteger()
        {
            return _value;
        }

		[[nodiscard]] constexpr const ValueType &AsInteger() const
        {
            return _value;
        }
    };
}