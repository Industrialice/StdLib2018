#include <StdCoreLib.hpp>
#include "TimeMoment.hpp"

using namespace StdLib;

//////////////////////
// TimeDifference64 //
//////////////////////

TimeDifference64 TimeDifference64::operator - (const TimeDifference64 &other) const
{
    TimeDifference64 ret;
    ret._counter = _counter - other._counter;
    return ret;
}

TimeDifference64 TimeDifference64::operator + (const TimeDifference64 &other) const
{
    TimeDifference64 ret;
    ret._counter = _counter + other._counter;
    return ret;
}

TimeDifference64 &TimeDifference64::operator -= (const TimeDifference64 &other)
{
    _counter -= other._counter;
    return *this;
}

TimeDifference64 &TimeDifference64::operator += (const TimeDifference64 &other)
{
    _counter += other._counter;
    return *this;
}

bool TimeDifference64::operator < (const TimeDifference64 &other) const
{
    return _counter < other._counter;
}

bool TimeDifference64::operator <= (const TimeDifference64 &other) const
{
    return _counter <= other._counter;
}

bool TimeDifference64::operator > (const TimeDifference64 &other) const
{
    return _counter > other._counter;
}

bool TimeDifference64::operator >= (const TimeDifference64 &other) const
{
    return _counter >= other._counter;
}

bool TimeDifference64::operator == (const TimeDifference64 &other) const
{
    return _counter == other._counter;
}

bool TimeDifference64::operator != (const TimeDifference64 &other) const
{
    return _counter != other._counter;
}

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

////////////////////
// TimeDifference //
////////////////////

TimeDifference TimeDifference::operator - (const TimeDifference &other) const
{
    TimeDifference ret;
    ret._counter = _counter - other._counter;
    return ret;
}

TimeDifference TimeDifference::operator + (const TimeDifference &other) const
{
    TimeDifference ret;
    ret._counter = _counter + other._counter;
    return ret;
}

TimeDifference &TimeDifference::operator -= (const TimeDifference &other)
{
    _counter -= other._counter;
    return *this;
}

TimeDifference &TimeDifference::operator += (const TimeDifference &other)
{
    _counter += other._counter;
    return *this;
}

bool TimeDifference::operator < (const TimeDifference &other) const
{
    return _counter < other._counter;
}

bool TimeDifference::operator <= (const TimeDifference &other) const
{
    return _counter <= other._counter;
}

bool TimeDifference::operator > (const TimeDifference &other) const
{
    return _counter > other._counter;
}

bool TimeDifference::operator >= (const TimeDifference &other) const
{
    return _counter >= other._counter;
}

bool TimeDifference::operator == (const TimeDifference &other) const
{
    return _counter == other._counter;
}

bool TimeDifference::operator != (const TimeDifference &other) const
{
    return _counter != other._counter;
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

////////////////
// TimeMoment //
////////////////

bool TimeMoment::HasValue() const
{
    return _counter > i64_min;
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