#pragma once

#include <cstdio>
#include <unordered_set>
#include <StdMiscellaneousLib.hpp>

#ifdef PLATFORM_WINDOWS
    #define NOGDICAPMASKS
    #define NOVIRTUALKEYCODES
    #define NOWINMESSAGES
    #define NOWINSTYLES
    #define NOSYSMETRICS
    #define NOMENUS
    #define NOICONS
    #define NOKEYSTATES
    #define NOSYSCOMMANDS
    #define NORASTEROPS
    #define NOSHOWWINDOW
    #define OEMRESOURCE
    #define NOATOM
    #define NOCLIPBOARD
    #define NOCOLOR
    #define NOCTLMGR
    #define NODRAWTEXT
    #define NOGDI
    #define NOKERNEL
    #define NOUSER
    #define NONLS
    #define NOMB
    #define NOMEMMGR
    #define NOMETAFILE
    #define NOMINMAX
    #define NOMSG
    #define NOOPENFILE
    #define NOSCROLL
    #define NOSERVICE
    #define NOSOUND
    #define NOTEXTMETRIC
    #define NOWH
    #define NOWINOFFSETS
    #define NOCOMM
    #define NOKANJI
    #define NOHELP
    #define NOPROFILER
    #define NODEFERWINDOWPOS
    #define NOMCX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

/*#ifdef PLATFORM_ANDROID
    #include <android/log.h>
#endif

#ifdef PLATFORM_ANDROID
    #define Logger::Message(...) __android_log_print(ANDROID_LOG_INFO, "StdLib", __VA_ARGS__)
#else
    #define Logger::Message(...) printf(__VA_ARGS__)
#endif*/

#include "Logger.hpp"

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

#ifdef _MSC_VER
    #define UTest(cond, ...) UnitTest<cond>(__VA_ARGS__, TOSTR(cond), TOSTR(__VA_ARGS__), __LINE__, __FILE__)
#else
    #define UTest(cond, ...) UnitTest<cond>(__VA_ARGS__, TOSTR(cond), "", __LINE__, __FILE__)
#endif