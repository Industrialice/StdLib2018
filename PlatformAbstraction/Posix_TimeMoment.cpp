#include "_PreHeader.hpp"
#include "TimeMoment.hpp"
#include <time.h>

using namespace StdLib;

static i64 CurrentTime();

namespace
{
    constexpr f32 Sec32ToCounter = 1'000'000'000.0f;
    constexpr f32 CounterToSec32 = 0.000'000'001f;
    constexpr f64 Sec64ToCounter = 1'000'000'000.0;
    constexpr f64 CounterToSec64 = 0.000'000'001;
}

TimeDifference::TimeDifference(f32 seconds)
{
    _counter = (i64)(seconds * Sec32ToCounter);
}

TimeDifference::TimeDifference(f64 seconds)
{
	_counter = (i64)(seconds * Sec64ToCounter);
}

f32 TimeDifference::ToSeconds() const
{
    return _counter * CounterToSec32;
}

TimeMoment TimeMoment::Now()
{
    TimeMoment ret;
    ret._counter = CurrentTime();
    return ret;
}

namespace StdLib::TimeMomentInitialization
{
    void Initialize()
    {}
}

i64 CurrentTime()
{
    timespec t;
    clockid_t id = CLOCK_MONOTONIC_RAW;
#ifdef PLATFORM_EMSCRIPTEN
    id = CLOCK_MONOTONIC;
#endif
    clock_gettime(id, &t);
    return t.tv_sec * 100000000ULL + (t.tv_nsec / 10); // TODO: possible overflow?
}