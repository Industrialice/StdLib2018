#include <StdCoreLib.hpp>
#include "TimeMoment.hpp"
#include <time.h>

using namespace StdLib;

static platformTimeCounter CurrentTime();

namespace
{
    constexpr f32 Sec32ToCounter = 1'000'000'000.0f;
    constexpr f32 CounterToSec32 = 0.000'000'001f;
    constexpr f64 Sec64ToCounter = 1'000'000'000.0;
    constexpr f64 CounterToSec64 = 0.000'000'001;
}

TimeDifference64::TimeDifference64(f64 seconds)
{
    _counter = (ui64)(seconds * Sec64ToCounter);
}

f64 TimeDifference64::ToSeconds() const
{
    return _counter * CounterToSec64;
}

TimeDifference::TimeDifference(f32 seconds)
{
    _counter = (ui64)(seconds * Sec32ToCounter);
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

platformTimeCounter CurrentTime()
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return t.tv_sec * 100000000ULL + (t.tv_nsec / 10);
}