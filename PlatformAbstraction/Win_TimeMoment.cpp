#include <StdCoreLib.hpp>
#include "TimeMoment.hpp"
#include <Windows.h>

using namespace StdLib;

namespace
{
    f32 CounterToSec32;
    f32 Sec32ToCounter;
    f64 CounterToSec64;
    f64 Sec64ToCounter;
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
    LARGE_INTEGER current;
    BOOL result = QueryPerformanceCounter(&current);
    ASSUME(result);
    TimeMoment ret;
    ret._counter = current.QuadPart;
    return ret;
}

namespace StdLib::TimeMomentInitialization
{
    void Initialize()
    {
        LARGE_INTEGER freq;
        BOOL result = QueryPerformanceFrequency(&freq);
        ASSUME(result);

        CounterToSec32 = 1.0f / freq.QuadPart;
        Sec32ToCounter = (f32)freq.QuadPart;
        CounterToSec64 = 1.0 / freq.QuadPart;
        Sec64ToCounter = (f64)freq.QuadPart;
    }
}