#include "_PreHeader.hpp"
#include "TimeMoment.hpp"

using namespace StdLib;

////////////////////
// TimeDifference //
////////////////////

TimeDifference::TimeDifference(i64 counter) : _counter(counter)
{}

TimeDifference TimeDifference::ToAbsolute() const
{
	return TimeDifference(abs(_counter));
}

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

#ifndef SPACESHIP_SUPPORTED
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
#endif

////////////////
// TimeMoment //
////////////////

namespace
{
	constexpr i64 EarliestCounter = i64_min + 1;
}

bool TimeMoment::HasValue() const
{
    return _counter > i64_min;
}

TimeDifference TimeMoment::operator - (const TimeMoment &other) const
{
    ASSUME(HasValue() && other.HasValue());
	i64 result;
	if (_counter == EarliestCounter)
	{
		result = EarliestCounter;
	}
	else if (other._counter == EarliestCounter)
	{
		result = i64_max;
	}
	else
	{
		result = _counter - other._counter;
	}
    return TimeDifference{result};
}

TimeMoment TimeMoment::operator + (const TimeDifference &difference) const
{
    ASSUME(HasValue());
	i64 result;
	if (_counter == EarliestCounter)
	{
		result = EarliestCounter;
	}
	else if (difference._counter == EarliestCounter)
	{
		result = _counter;
	}
	else
	{
		result = _counter + difference._counter;
	}
    return TimeMoment{result};
}

TimeMoment &TimeMoment::operator += (const TimeDifference &difference)
{
    ASSUME(HasValue());
	if (_counter != EarliestCounter && difference._counter != EarliestCounter)
	{
		_counter += difference._counter;
	}
    return *this;
}

TimeMoment TimeMoment::operator - (const TimeDifference &difference) const
{
    ASSUME(HasValue());
	if (_counter == EarliestCounter || difference._counter == EarliestCounter)
	{
		return *this;
	}
	else
	{
		return TimeMoment{_counter - difference._counter};
	}
}

TimeMoment &TimeMoment::operator -= (const TimeDifference &difference)
{
    ASSUME(HasValue());
	if (_counter != EarliestCounter && difference._counter != EarliestCounter)
	{
		_counter -= difference._counter;
	}
    return *this;
}

#ifndef SPACESHIP_SUPPORTED
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
#endif

TimeMoment::operator bool() const
{
    return HasValue();
}