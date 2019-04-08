#include "_PreHeader.hpp"
#include "TimeMoment.hpp"
#include <time.h>

using namespace StdLib;

static i64 CurrentTime();

namespace
{
    constexpr i64 FreqInt = 1'000'000'000;
    constexpr i64 FreqMInt = 60'000'000'000;
    constexpr f64 FreqFP64 = 1'000'000'000.0;
    constexpr f64 FreqMFP64 = 60'000'000'000.0;
    constexpr f64 FreqMSFP64 = 1'000'000.0;
    constexpr f64 FreqUSFP64 = 1'000.0;
    constexpr f32 RevFreqFP32 = 0.000'000'001f;
    constexpr f64 RevFreqFP64 = 0.000'000'001;
    constexpr f32 RevFreqMSFP32 = 0.000'001f;
    constexpr f64 RevFreqMSFP64 = 0.000'001;
    constexpr f32 RevFreqUSFP32 = 0.001f;
    constexpr f64 RevFreqUSFP64 = 0.001;
}

TimeDifference::TimeDifference(MinutesFP64 time)
{
    _counter = (i64)(time * FreqMFP64);
}

TimeDifference::TimeDifference(MinutesI64 time)
{
    _counter = time * FreqMInt;
}

TimeDifference::TimeDifference(SecondsFP64 time)
{
    _counter = (i64)(time * FreqFP64);
}

TimeDifference::TimeDifference(SecondsI64 time)
{
    _counter = time * FreqInt;
}

TimeDifference::TimeDifference(MilliSecondsFP64 time)
{
    _counter = (i64)(time * FreqMSFP64);
}

TimeDifference::TimeDifference(MilliSecondsI64 time)
{
    _counter = time * FreqInt / 1'000;
}

TimeDifference::TimeDifference(MicroSecondsFP64 time)
{
    _counter = (i64)(time * FreqUSFP64);
}

TimeDifference::TimeDifference(MicroSecondsI64 time)
{
    _counter = time * FreqInt / 1'000'000;
}

f32 TimeDifference::ToSec() const
{
    return _counter * RevFreqFP32;
}

f64 TimeDifference::ToSec(f64) const
{
    return _counter * RevFreqFP64;
}

i32 TimeDifference::ToSec(i32) const
{
    i64 result = ToSec(i64());
    ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
    return (i32)result;
}

i64 TimeDifference::ToSec(i64) const
{
    return _counter / FreqInt;
}

f32 TimeDifference::ToMSec() const
{
    return _counter * RevFreqMSFP32;
}

f64 TimeDifference::ToMSec(f64) const
{
    return _counter * RevFreqMSFP64;
}

i32 TimeDifference::ToMSec(i32) const
{
    i64 result = ToMSec(i64());
    ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
    return (i32)result;
}

i64 TimeDifference::ToMSec(i64) const
{
    return (_counter * 1'000) / FreqInt;
}

f32 TimeDifference::ToUSec() const
{
    return _counter * RevFreqUSFP32;
}

f64 TimeDifference::ToUSec(f64) const
{
    return _counter * RevFreqUSFP64;
}

i32 TimeDifference::ToUSec(i32) const
{
    i64 result = ToUSec(i64());
    ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
    return (i32)result;
}

i64 TimeDifference::ToUSec(i64) const
{
    return (_counter * 1'000'000) / FreqInt;
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