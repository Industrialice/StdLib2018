#pragma once

#include <cstdio>

#include <StdCoreLib.hpp>

#ifdef PLATFORM_WINDOWS
    #define NOMINMAX
    #include <Windows.h>
#endif

#ifdef PLATFORM_ANDROID
    #include <android/log.h>
#endif

#ifdef PLATFORM_ANDROID
    #define PRINTLOG(...) __android_log_print(ANDROID_LOG_INFO, "StdLib", __VA_ARGS__)
#else
    #define PRINTLOG(...) printf(__VA_ARGS__)
#endif

enum UnitTestType
{
    LeftLesser,
    LeftLesserEqual,
    Equal,
    LeftGreaterEqual,
    LeftGreater,
    NotEqual
};

template <typename T> void Assume(T &&condition, const char *condStr, const char *argsStr, int line, const char *file)
{
    if (!(condition))
    {
        printf("condition %s failed for arguments %s, file %s line %i\n", condStr, argsStr, file, line);
        HARDBREAK;
    }
}

template <UnitTestType testType, typename T, typename E> void UnitTest(T &&left, E &&right, const char *condStr, const char *argsStr, int line, const char *file)
{
    if constexpr (testType == LeftLesser) Assume(left < right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftLesserEqual) Assume(left <= right, condStr, argsStr, line, file);
    else if constexpr (testType == Equal) Assume(left == right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftGreaterEqual) Assume(left >= right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftGreater) Assume(left > right, condStr, argsStr, line, file);
    else if constexpr (testType == NotEqual) Assume(left != right, condStr, argsStr, line, file);
}

template <bool isTrue, typename T> void UnitTest(T &&value, const char *condStr, const char *argsStr, int line, const char *file)
{
    if constexpr (isTrue)
    {
        Assume(value, condStr, argsStr, line, file);
    }
    else
    {
        Assume(!value, condStr, argsStr, line, file);
    }
}

#define UTest(cond, ...) UnitTest<cond>(__VA_ARGS__, TOSTR(cond), /*TOSTR(__VA_ARGS__)**/"", __LINE__, __FILE__)