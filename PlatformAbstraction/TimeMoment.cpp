#include "_PreHeader.hpp"
#include "TimeMoment.hpp"

using namespace StdLib;

//////////////////////
// TimeDifference64 //
//////////////////////

TimeDifference64::TimeDifference64(i64 counter) : _counter(counter)
{}

TimeDifference64 TimeDifference64::operator - (const TimeDifference64 &other) const
{
    return TimeDifference64{_counter - other._counter};
}

TimeDifference64 TimeDifference64::operator + (const TimeDifference64 &other) const
{
    return TimeDifference64{_counter + other._counter};
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
    return TimeDifference{_counter};
}

TimeDifference64::operator TimeDifference() const
{
    return As32();
}

////////////////////
// TimeDifference //
////////////////////

TimeDifference::TimeDifference(i64 counter) : _counter(counter)
{}

TimeDifference TimeDifference::operator - (const TimeDifference &other) const
{
    return TimeDifference{_counter - other._counter};
}

TimeDifference TimeDifference::operator + (const TimeDifference &other) const
{
    return TimeDifference{_counter + other._counter};
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
    return TimeDifference64{_counter};
}

TimeDifference::operator TimeDifference64() const
{
    return As64();
}

////////////////
// TimeMoment //
////////////////

TimeMoment::TimeMoment(i64 counter) : _counter(counter)
{}

bool TimeMoment::HasValue() const
{
    return _counter > i64_min;
}

TimeDifference TimeMoment::operator - (const TimeMoment &other) const
{
    ASSUME(HasValue());
    return TimeDifference{_counter - other._counter};
}

TimeMoment TimeMoment::operator + (const TimeDifference &difference) const
{
    ASSUME(HasValue());
    return TimeMoment{_counter + difference._counter};
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
    return TimeMoment{_counter - difference._counter};
}

TimeMoment &TimeMoment::operator -= (const TimeDifference &difference)
{
    ASSUME(HasValue());
    _counter -= difference._counter;
    return *this;
}

bool TimeMoment::operator < (const TimeMoment &other) const
{
    return _counter < other._counter;
}

bool TimeMoment::operator <= (const TimeMoment &other) const
{
    return _counter <= other._counter;
}

bool TimeMoment::operator > (const TimeMoment &other) const
{
    return _counter > other._counter;
}

bool TimeMoment::operator >= (const TimeMoment &other) const
{
    return _counter >= other._counter;
}

bool TimeMoment::operator == (const TimeMoment &other) const
{
    return _counter == other._counter;
}

bool TimeMoment::operator != (const TimeMoment &other) const
{
    return _counter != other._counter;
}

TimeMoment::operator bool() const
{
    return HasValue();
}
