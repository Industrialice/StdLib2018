#include "_PreHeader.hpp"
#include "StdCoreLib.hpp"

using namespace StdLib;

namespace
{
    std::function<void(const char *, i32, i32)> SoftBreakCallback{};
}

namespace StdLib
{
    f32 DefaultF32Epsilon{};
}

static void DefaultSoftBreakCallback(const char *file, i32 line, i32 counter);

void _Private::SoftBreak(const char *file, i32 line, i32 counter)
{
    ASSUME(SoftBreakCallback);
    SoftBreakCallback(file, line, counter);
}

void Initialization::CoreInitialize(const CoreParameters &parameters)
{
    SoftBreakCallback = parameters.softBreakCallback;
    if (!SoftBreakCallback)
    {
        SoftBreakCallback = DefaultSoftBreakCallback;
    }

    DefaultF32Epsilon = parameters.defaultF32Epsilon;
}

void DefaultSoftBreakCallback(const char *file, i32 line, i32 counter)
{
#ifdef PLATFORM_WINDOWS
    class BreakInfo
    {
        i32 _line{};
        i32 _counter{};
        std::string _file{};

    public:
        BreakInfo(std::string_view file, i32 line, i32 counter) : _file{file}, _line{line}, _counter{counter}
        {}

        bool operator < (const BreakInfo &other) const
        {
            return std::make_tuple(_line, _counter, _file) < std::make_tuple(other._line, other._counter, other._file);
        }
    };

    static std::set<BreakInfo> breaksToIgnore;

    BreakInfo newBreak{file, line, counter};
    auto it = breaksToIgnore.find(newBreak);
    if (it != breaksToIgnore.end())
    {
        return;
    }

    std::string text = "Soft break occured in file "s + file + " at line " + std::to_string(line) + "\nAbort - abort program execution\nRetry - break\nIgnore - ignore this break and stop reporting it";

    int result = MessageBoxA(0, text.c_str(), "Soft break has occured", MB_ABORTRETRYIGNORE);
    switch (result)
    {
    case IDABORT:
        exit(1);
        break;
    case IDRETRY:
        __debugbreak();
        break;
    case IDIGNORE:
        breaksToIgnore.insert(newBreak);
        break;
    }
#elif defined(PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_ERROR, "soft break", "%s %i", file, line);
#else
    fprintf(stderr, "soft break %s %i\n", file, line);
#endif
}