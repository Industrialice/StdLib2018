#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib
{
	template <typename T, typename E> struct _TimeBase
	{
		E value;

		_TimeBase(E _value) : value(_value)	{}

		operator E () const
		{
			return value;
		}

		T operator - () const 
		{ 
            return -value;
		}
	};

    struct MinutesFP64 : _TimeBase<MinutesFP64, f64>
    {
        using _TimeBase<MinutesFP64, f64>::_TimeBase;
    };
    struct MinutesI64 : _TimeBase<MinutesI64, i64>
    {
        using _TimeBase<MinutesI64, i64>::_TimeBase;
    };
	struct SecondsFP64 : _TimeBase<SecondsFP64, f64> 
	{
		using _TimeBase<SecondsFP64, f64>::_TimeBase;
	};
	struct SecondsI64 : _TimeBase<SecondsI64, i64> 
	{
		using _TimeBase<SecondsI64, i64>::_TimeBase;
	};
	struct MilliSecondsFP64 : _TimeBase<MilliSecondsFP64, f64>
	{
		using _TimeBase<MilliSecondsFP64, f64>::_TimeBase;
	};
	struct MilliSecondsI64 : _TimeBase<MilliSecondsI64, i64>
	{
		using _TimeBase<MilliSecondsI64, i64>::_TimeBase;
	};
	struct MicroSecondsFP64 : _TimeBase<MicroSecondsFP64, f64>
	{
		using _TimeBase<MicroSecondsFP64, f64>::_TimeBase;
	};
	struct MicroSecondsI64 : _TimeBase<MicroSecondsI64, i64>
	{
		using _TimeBase<MicroSecondsI64, i64>::_TimeBase;
	};

    inline MinutesFP64 operator "" _m(long double value)
    {
        return {(f64)value};
    }
    inline MinutesI64 operator "" _m(unsigned long long value)
    {
        return {(i64)value};
    }
	inline SecondsFP64 operator "" _s(long double value)
	{
		return {(f64)value};
	}
	inline SecondsI64 operator "" _s(unsigned long long value)
	{
		return {(i64)value};
	}
	inline MilliSecondsFP64 operator "" _ms(long double value)
	{
		return {(f64)value};
	}
	inline MilliSecondsI64 operator "" _ms(unsigned long long value)
	{
		return {(i64)value};
	}
	inline MicroSecondsFP64 operator "" _us(long double value)
	{
		return {(f64)value};
	}
	inline MicroSecondsI64 operator "" _us(unsigned long long value)
	{
		return {(i64)value};
	}

    class TimeDifference
    {
        friend class TimeMoment;
         
        i64 _counter = i64_min;

        explicit TimeDifference(i64 counter);

    public:
        TimeDifference() = delete;
        TimeDifference(MinutesFP64 time);
        TimeDifference(MinutesI64 time);
		TimeDifference(SecondsFP64 time);
		TimeDifference(SecondsI64 time);
		TimeDifference(MilliSecondsFP64 time);
		TimeDifference(MilliSecondsI64 time);
		TimeDifference(MicroSecondsFP64 time);
		TimeDifference(MicroSecondsI64 time);

        [[nodiscard]] f32 ToSec() const;
		[[nodiscard]] f64 ToSec(f64) const;
		[[nodiscard]] i32 ToSec(i32) const;
		[[nodiscard]] i64 ToSec(i64) const;

		[[nodiscard]] f32 ToMSec() const;
		[[nodiscard]] f64 ToMSec(f64) const;
		[[nodiscard]] i32 ToMSec(i32) const;
		[[nodiscard]] i64 ToMSec(i64) const;

		[[nodiscard]] f32 ToUSec() const;
		[[nodiscard]] f64 ToUSec(f64) const;
		[[nodiscard]] i32 ToUSec(i32) const;
		[[nodiscard]] i64 ToUSec(i64) const;

        [[nodiscard]] TimeDifference operator - (const TimeDifference &other) const;
        [[nodiscard]] TimeDifference operator + (const TimeDifference &other) const;
        TimeDifference &operator -= (const TimeDifference &other);
        TimeDifference &operator += (const TimeDifference &other);

        [[nodiscard]] bool operator < (const TimeDifference &other) const;
        [[nodiscard]] bool operator <= (const TimeDifference &other) const;
        [[nodiscard]] bool operator > (const TimeDifference &other) const;
        [[nodiscard]] bool operator >= (const TimeDifference &other) const;
        [[nodiscard]] bool operator == (const TimeDifference &other) const;
        [[nodiscard]] bool operator != (const TimeDifference &other) const;
    };

    // note that TimeMoment is undefined after the default contructor had 
    // been called
	// using undefined TimeMoment for anything but operator <=>
    // is an error and will trigger an exception in debug mode
    // use TimeMoment::New() to create a TimeMoment for the current moment
    class TimeMoment
    {
        i64 _counter = i64_min;

        explicit TimeMoment(i64 counter);

    public:
        TimeMoment() = default;
        bool HasValue() const;

        [[nodiscard]] TimeDifference operator - (const TimeMoment &other) const;
        [[nodiscard]] TimeMoment operator + (const TimeDifference &difference) const;
        TimeMoment &operator += (const TimeDifference &difference);
        [[nodiscard]] TimeMoment operator - (const TimeDifference &difference) const;
        TimeMoment &operator -= (const TimeDifference &difference);

        [[nodiscard]] bool operator < (const TimeMoment &other) const;
        [[nodiscard]] bool operator <= (const TimeMoment &other) const;
        [[nodiscard]] bool operator > (const TimeMoment &other) const;
        [[nodiscard]] bool operator >= (const TimeMoment &other) const;
        [[nodiscard]] bool operator == (const TimeMoment &other) const;
        [[nodiscard]] bool operator != (const TimeMoment &other) const;

        [[nodiscard]] explicit operator bool() const;

        static TimeMoment Now();
    };
}