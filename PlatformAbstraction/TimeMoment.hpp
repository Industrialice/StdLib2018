#pragma once

#include "PlatformAbstractionCoreTypes.hpp"

namespace StdLib
{
    class TimeDifference;

    class TimeDifference64
    {
        friend TimeDifference;

        i64 _counter = i64_min;

        explicit TimeDifference64(i64 counter);

    public:
        TimeDifference64() = delete;
        TimeDifference64(f64 seconds);

        [[nodiscard]] f64 ToSeconds() const;

        [[nodiscard]] TimeDifference64 operator - (const TimeDifference64 &other) const;
        [[nodiscard]] TimeDifference64 operator + (const TimeDifference64 &other) const;
        TimeDifference64 &operator -= (const TimeDifference64 &other);
        TimeDifference64 &operator += (const TimeDifference64 &other);

        [[nodiscard]] bool operator < (const TimeDifference64 &other) const;
        [[nodiscard]] bool operator <= (const TimeDifference64 &other) const;
        [[nodiscard]] bool operator > (const TimeDifference64 &other) const;
        [[nodiscard]] bool operator >= (const TimeDifference64 &other) const;
        [[nodiscard]] bool operator == (const TimeDifference64 &other) const;
        [[nodiscard]] bool operator != (const TimeDifference64 &other) const;

        [[nodiscard]] TimeDifference As32() const;
        explicit operator TimeDifference() const;
    };

    class TimeDifference
    {
        friend class TimeMoment;
        friend TimeDifference64;
         
        i64 _counter = i64_min;

        explicit TimeDifference(i64 counter);

    public:
        TimeDifference() = delete;
        TimeDifference(f32 seconds);

        [[nodiscard]] f32 ToSeconds() const;

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

        [[nodiscard]] TimeDifference64 As64() const;
        explicit operator TimeDifference64() const;
    };

    // note that TimeMoment is undefined after the default contructor had 
    // been called using undefined TimeMoment for anything but operator <=>
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