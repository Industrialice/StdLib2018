#include "_PreHeader.hpp"
#include <StdCoreLib.hpp>
#include "Logger.hpp"
#include <stdarg.h>

using namespace StdLib;

#if defined(PLATFORM_ANDROID)
    void OnLogMessage(const char *message);
    void PopLastMessage(unsigned int length);
#endif

bool Logger::IsMessagePoppingSupported()
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_ANDROID)
    return true;
#else
    return false;
#endif
}

void Logger::PopLastMessage()
{
#if defined(PLATFORM_WINDOWS)
    static HANDLE outputHandle;
    if (!outputHandle)
    {
        outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(outputHandle, &info);
    COORD newCoord = {0, info.dwCursorPosition.Y};
    SetConsoleCursorPosition(outputHandle, newCoord);
    static char message[4096];
    if (!message[0])
    {
        std::fill(message, message + CountOf(message), ' ');
    }
    DWORD written;
    WriteConsoleA(outputHandle, message, std::min<ui32>(CountOf(message), _lastMessageLength), &written, 0);
    SetConsoleCursorPosition(outputHandle, newCoord);
#elif defined(PLATFORM_ANDROID)
    ::PopLastMessage(_lastMessageLength);
#else
    HARDBREAK;
#endif
}

void Logger::Message(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#if defined(PLATFORM_WINDOWS)
    _lastMessageLength = (ui32)vprintf(fmt, args);
#elif defined(PLATFORM_ANDROID)
    char buf[4096];
    _lastMessageLength = vsnprintf(buf, CountOf(buf), fmt, args);
    buf[CountOf(buf) - 1] = '\0';
    OnLogMessage(buf);
#else
    HARDBREAK;
#endif
    va_end(args);
}
