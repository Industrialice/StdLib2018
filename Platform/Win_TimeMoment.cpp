#include "_PreHeader.hpp"
#include "TimeMoment.hpp"

using namespace StdLib;

namespace
{
    bool IsInitialized;
	i64 FreqInt;
	f64 FreqFP64;
    i64 FreqMInt;
    f64 FreqMFP64;
	f64 FreqMSFP64;
	f64 FreqUSFP64;
    f32 RevFreqFP32;
    f64 RevFreqFP64;
	f32 RevFreqMSFP32;
	f64 RevFreqMSFP64;
	f32 RevFreqUSFP32;
	f64 RevFreqUSFP64;
}

TimeDifference::TimeDifference(TimeMinutesFP64 time)
{
    ASSUME(IsInitialized);
    _counter = static_cast<i64>(time * FreqMFP64);
}

TimeDifference::TimeDifference(TimeMinutesI64 time)
{
    ASSUME(IsInitialized);
    _counter = time * FreqMInt;
}

TimeDifference::TimeDifference(TimeSecondsFP64 time)
{
    ASSUME(IsInitialized);
    _counter = static_cast<i64>(time * FreqFP64);
}

TimeDifference::TimeDifference(TimeSecondsI64 time)
{
    ASSUME(IsInitialized);
    _counter = time * FreqInt;
}

TimeDifference::TimeDifference(TimeMilliSecondsFP64 time)
{
    ASSUME(IsInitialized);
    _counter = static_cast<i64>(time * FreqMSFP64);
}

TimeDifference::TimeDifference(TimeMilliSecondsI64 time)
{
    ASSUME(IsInitialized);
    _counter = time * FreqInt / 1'000;
}

TimeDifference::TimeDifference(TimeMicroSecondsFP64 time)
{
    ASSUME(IsInitialized);
    _counter = static_cast<i64>(time * FreqUSFP64);
}

TimeDifference::TimeDifference(TimeMicroSecondsI64 time)
{
    ASSUME(IsInitialized);
    _counter = time * FreqInt / 1'000'000;
}

f32 TimeDifference::ToSec() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqFP32;
}

f64 TimeDifference::ToSec_f64() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqFP64;
}

i32 TimeDifference::ToSec_i32() const
{
	i64 result = ToSec_i64();
	ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
	return static_cast<i32>(result);
}

i64 TimeDifference::ToSec_i64() const
{
    ASSUME(IsInitialized);
    return _counter / FreqInt;
}

f32 TimeDifference::ToMSec() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqMSFP32;
}

f64 TimeDifference::ToMSec_f64() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqMSFP64;
}

i32 TimeDifference::ToMSec_i32() const
{
	i64 result = ToMSec_i64();
	ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
	return static_cast<i32>(result);
}

i64 TimeDifference::ToMSec_i64() const
{
    ASSUME(IsInitialized);
    return (_counter * 1'000) / FreqInt;
}

f32 TimeDifference::ToUSec() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqUSFP32;
}

f64 TimeDifference::ToUSec_f64() const
{
    ASSUME(IsInitialized);
    return _counter * RevFreqUSFP64;
}

i32 TimeDifference::ToUSec_i32() const
{
	i64 result = ToUSec_i64();
	ASSUME(result <= std::numeric_limits<i32>::max() && result >= std::numeric_limits<i32>::min());
	return static_cast<i32>(result);
}

i64 TimeDifference::ToUSec_i64() const
{
    ASSUME(IsInitialized);
    return (_counter * 1'000'000) / FreqInt;
}

TimeMoment TimeMoment::Now()
{
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    TimeMoment ret;
    ret._counter = current.QuadPart;
    return ret;
}

namespace StdLib::TimeMomentInitialization
{
    void Initialize()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

		FreqInt = freq.QuadPart;
		FreqFP64 = static_cast<f64>(FreqInt);
        FreqMInt = FreqInt * 60;
        FreqMFP64 = FreqInt * 60.0;
		FreqMSFP64 = FreqInt * 0.001;
		FreqUSFP64 = FreqInt * 0.000001;
        RevFreqFP32 = 1.0f / FreqInt;
        RevFreqFP64 = 1.0 / FreqInt;
		RevFreqMSFP32 = 1'000.0f / FreqInt;
		RevFreqMSFP64 = 1'000.0 / FreqInt;
		RevFreqUSFP32 = 1'000'000.0f / FreqInt;
		RevFreqUSFP64 = 1'000'000.0 / FreqInt;

        IsInitialized = true;
    }
}