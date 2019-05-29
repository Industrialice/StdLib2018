#pragma once

namespace StdLib
{
	template <typename T, typename E> struct _TimeBase
	{
		E value;

		constexpr _TimeBase(E _value) : value(_value)	{}

        constexpr operator E () const
		{
			return value;
		}

        constexpr T operator - () const
		{ 
            return -value;
		}
	};

    struct TimeMinutesFP64 : _TimeBase<TimeMinutesFP64, f64>
    {
        using _TimeBase<TimeMinutesFP64, f64>::_TimeBase;
    };
    struct TimeMinutesI64 : _TimeBase<TimeMinutesI64, i64>
    {
        using _TimeBase<TimeMinutesI64, i64>::_TimeBase;
    };
	struct TimeSecondsFP64 : _TimeBase<TimeSecondsFP64, f64> 
	{
		using _TimeBase<TimeSecondsFP64, f64>::_TimeBase;
	};
	struct TimeSecondsI64 : _TimeBase<TimeSecondsI64, i64> 
	{
		using _TimeBase<TimeSecondsI64, i64>::_TimeBase;
	};
	struct TimeMilliSecondsFP64 : _TimeBase<TimeMilliSecondsFP64, f64>
	{
		using _TimeBase<TimeMilliSecondsFP64, f64>::_TimeBase;
	};
	struct TimeMilliSecondsI64 : _TimeBase<TimeMilliSecondsI64, i64>
	{
		using _TimeBase<TimeMilliSecondsI64, i64>::_TimeBase;
	};
	struct TimeMicroSecondsFP64 : _TimeBase<TimeMicroSecondsFP64, f64>
	{
		using _TimeBase<TimeMicroSecondsFP64, f64>::_TimeBase;
	};
	struct TimeMicroSecondsI64 : _TimeBase<TimeMicroSecondsI64, i64>
	{
		using _TimeBase<TimeMicroSecondsI64, i64>::_TimeBase;
	};

    constexpr inline TimeMinutesFP64 operator "" _m(long double value)
    {
        return {(f64)value};
    }
    constexpr inline TimeMinutesI64 operator "" _m(unsigned long long value)
    {
        return {(i64)value};
    }
    constexpr inline TimeSecondsFP64 operator "" _s(long double value)
	{
		return {(f64)value};
	}
    constexpr inline TimeSecondsI64 operator "" _s(unsigned long long value)
	{
		return {(i64)value};
	}
    constexpr inline TimeMilliSecondsFP64 operator "" _ms(long double value)
	{
		return {(f64)value};
	}
    constexpr inline TimeMilliSecondsI64 operator "" _ms(unsigned long long value)
	{
		return {(i64)value};
	}
    constexpr inline TimeMicroSecondsFP64 operator "" _us(long double value)
	{
		return {(f64)value};
	}
    constexpr inline TimeMicroSecondsI64 operator "" _us(unsigned long long value)
	{
		return {(i64)value};
	}

    class TimeDifference
    {
        friend class TimeMoment;
         
        i64 _counter = 0;

        explicit TimeDifference(i64 counter);

    public:
        TimeDifference() = default;
        TimeDifference(TimeMinutesFP64 time);
        TimeDifference(TimeMinutesI64 time);
		TimeDifference(TimeSecondsFP64 time);
		TimeDifference(TimeSecondsI64 time);
		TimeDifference(TimeMilliSecondsFP64 time);
		TimeDifference(TimeMilliSecondsI64 time);
		TimeDifference(TimeMicroSecondsFP64 time);
		TimeDifference(TimeMicroSecondsI64 time);

        [[nodiscard]] f32 ToSec() const;
		[[nodiscard]] f64 ToSec_f64() const;
		[[nodiscard]] i32 ToSec_i32() const;
		[[nodiscard]] i64 ToSec_i64() const;

		[[nodiscard]] f32 ToMSec() const;
		[[nodiscard]] f64 ToMSec_f64() const;
		[[nodiscard]] i32 ToMSec_i32() const;
		[[nodiscard]] i64 ToMSec_i64() const;

		[[nodiscard]] f32 ToUSec() const;
		[[nodiscard]] f64 ToUSec_f64() const;
		[[nodiscard]] i32 ToUSec_i32() const;
		[[nodiscard]] i64 ToUSec_i64() const;

        [[nodiscard]] TimeDifference operator - (const TimeDifference &other) const;
        [[nodiscard]] TimeDifference operator + (const TimeDifference &other) const;
        TimeDifference &operator -= (const TimeDifference &other);
        TimeDifference &operator += (const TimeDifference &other);

#ifdef SPACESHIP_SUPPORTED
		[[nodiscard]] auto operator <=> (const TimeDifference &) const = default;
#else
		[[nodiscard]] bool operator < (const TimeDifference &other) const;
		[[nodiscard]] bool operator <= (const TimeDifference &other) const;
		[[nodiscard]] bool operator > (const TimeDifference &other) const;
		[[nodiscard]] bool operator >= (const TimeDifference &other) const;
		[[nodiscard]] bool operator == (const TimeDifference &other) const;
		[[nodiscard]] bool operator != (const TimeDifference &other) const;
#endif
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

#ifdef SPACESHIP_SUPPORTED
		[[nodiscard]] auto operator <=> (const TimeMoment &) const = default;
#else
		[[nodiscard]] bool operator < (const TimeMoment &other) const;
		[[nodiscard]] bool operator <= (const TimeMoment &other) const;
		[[nodiscard]] bool operator > (const TimeMoment &other) const;
		[[nodiscard]] bool operator >= (const TimeMoment &other) const;
		[[nodiscard]] bool operator == (const TimeMoment &other) const;
		[[nodiscard]] bool operator != (const TimeMoment &other) const;
#endif

        [[nodiscard]] explicit operator bool() const;

		[[nodiscard]] static TimeMoment Now();
    };
}