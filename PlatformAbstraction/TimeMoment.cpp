#include <StdCoreLib.hpp>
#include "TimeMoment.hpp"

using namespace StdLib;

TimeDifference TimeDifference64::As32() const
{
    TimeDifference ret;
    ret._counter = _counter;
    return ret;
}

TimeDifference64::operator TimeDifference() const
{
    return As32();
}

TimeDifference64 TimeDifference::As64() const
{
    TimeDifference64 ret;
    ret._counter = _counter;
    return ret;
}

TimeDifference::operator TimeDifference64() const
{
    return As64();
}

bool TimeMoment::HasValue() const
{
    return _counter != 0;
}

TimeDifference TimeMoment::operator - (const TimeMoment &other) const
{
    TimeDifference ret;
    ret._counter = _counter - other._counter;
    return ret;
}

TimeMoment TimeMoment::operator + (const TimeDifference &difference) const
{
    ASSUME(HasValue());
    TimeMoment ret;
    ret._counter = _counter + difference._counter;
    return ret;
}

TimeMoment &TimeMoment::operator += (const TimeDifference &difference)
{
    ASSUME(HasValue());
    _counter += difference._counter;
    return *this;
}

TimeMoment TimeMoment::operator - (const TimeDifference &difference) const
{
    ASSUME(HasValue());
    TimeMoment ret;
    ret._counter = _counter - difference._counter;
    return ret;
}

TimeMoment &TimeMoment::operator -= (const TimeDifference &difference)
{
    ASSUME(HasValue());
    _counter -= difference._counter;
    return *this;
}

bool TimeMoment::operator < (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter < other._counter;
}

bool TimeMoment::operator <= (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter <= other._counter;
}

bool TimeMoment::operator > (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter > other._counter;
}

bool TimeMoment::operator >= (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter >= other._counter;
}

bool TimeMoment::operator == (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter == other._counter;
}

bool TimeMoment::operator != (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
    return _counter != other._counter;
}