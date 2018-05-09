#pragma once

namespace StdLib::_Private
{
    void SoftBreak(const char *file, i32 line, i32 counter);
}

#ifdef DEBUG
    #define HARDBREAK _RAISE_EXCEPTION
    #define SOFTBREAK _Private::SoftBreak(__FILE__, __LINE__, __COUNTER__)
    #define UNREACHABLE _RAISE_EXCEPTION
    #define ASSUME(condition) do { if (!(condition)) HARDBREAK; } while(0)
#else
    #define HARDBREAK _UNREACHABLE
    #define SOFTBREAK
    #define UNREACHABLE _UNREACHABLE
    #define ASSUME(condition) _ASSUME(condition)
#endif

#define NOIMPL HARDBREAK

#define TOSTR(code) #code
#define CONCAT(first, second) first##second