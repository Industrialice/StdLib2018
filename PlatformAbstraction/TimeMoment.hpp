#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib
{
    class TimeDifference;

    class TimeDifference64
    {
        friend TimeDifference;

        platformTimeCounter _counter{};

    public:
        TimeDifference64() = default;
        TimeDifference64(f64 seconds);

        f64 ToSeconds() const;

        TimeDifference As32() const;
        explicit operator TimeDifference() const;
    };

    class TimeDifference
    {
        friend class TimeMoment;
        friend TimeDifference64;
         
        platformTimeCounter _counter{};

    public:
        TimeDifference() = default;
        TimeDifference(f32 seconds);

        f32 ToSeconds() const;

        TimeDifference64 As64() const;
        explicit operator TimeDifference64() const;
    };

    // note that TimeMoment is undefined after the default contructor had been called
    // using undefined TimeMoment is an error and will trigger an exception in debug mode
    // use TimeMoment::New() to create a TimeMoment for the current moment
    class TimeMoment
    {
        platformTimeCounter _counter{};

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