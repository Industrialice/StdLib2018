#include "_PreHeader.hpp"
#include "TimeMoment.hpp"

using namespace StdLib;

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