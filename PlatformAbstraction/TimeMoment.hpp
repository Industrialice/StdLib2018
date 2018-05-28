#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib
{
    class TimeDifference;

    class TimeDifference64
    {
        friend TimeDifference;

        i64 _counter = i64_min;

    public:
        TimeDifference64() = default;
        TimeDifference64(f64 seconds);

        f64 ToSeconds() const;

        TimeDifference64 operator - (const TimeDifference64 &other) const;
        TimeDifference64 operator + (const TimeDifference64 &other) const;
        TimeDifference64 &operator -= (const TimeDifference64 &other);
        TimeDifference64 &operator += (const TimeDifference64 &other);

        bool operator < (const TimeDifference64 &other) const;
        bool operator <= (const TimeDifference64 &other) const;
        bool operator > (const TimeDifference64 &other) const;
        bool operator >= (const TimeDifference64 &other) const;
        bool operator == (const TimeDifference64 &other) const;
        bool operator != (const TimeDifference64 &other) const;

        TimeDifference As32() const;
        explicit operator TimeDifference() const;
    };

    class TimeDifference
    {
        friend class TimeMoment;
        friend TimeDifference64;
         
        i64 _counter = i64_min;

    public:
        TimeDifference() = default;
        TimeDifference(f32 seconds);

        f32 ToSeconds() const;

        TimeDifference operator - (const TimeDifference &other) const;
        TimeDifference operator + (const TimeDifference &other) const;
        TimeDifference &operator -= (const TimeDifference &other);
        TimeDifference &operator += (const TimeDifference &other);

        bool operator < (const TimeDifference &other) const;
        bool operator <= (const TimeDifference &other) const;
        bool operator > (const TimeDifference &other) const;
        bool operator >= (const TimeDifference &other) const;
        bool operator == (const TimeDifference &other) const;
        bool operator != (const TimeDifference &other) const;

        TimeDifference64 As64() const;
        explicit operator TimeDifference64() const;
    };

    // note that TimeMoment is undefined after the default contructor had 
    // been called using undefined TimeMoment for anything but operator <=>
    // is an error and will trigger an exception in debug mode
    // use TimeMoment::New() to create a TimeMoment for the current moment
    class TimeMoment
    {
        i64 _counter = i64_min;

    public:
        TimeMoment() = default;
        bool HasValue() const;

        TimeDifference operator - (const TimeMoment &other) const;
        TimeMoment operator + (const TimeDifference &difference) const;
        TimeMoment &operator += (const TimeDifference &difference);
        TimeMoment operator - (const TimeDifference &difference) const;
        TimeMoment &operator -= (const TimeDifference &difference);

        bool operator < (const TimeMoment &other) const;
        bool operator <= (const TimeMoment &other) const;
        bool operator > (const TimeMoment &other) const;
        bool operator >= (const TimeMoment &other) const;
        bool operator == (const TimeMoment &other) const;
        bool operator != (const TimeMoment &other) const;

        static TimeMoment Now();
    };
}