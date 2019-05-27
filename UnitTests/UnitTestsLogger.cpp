#include "_PreHeader.hpp"
#include <StdCoreLib.hpp>
#include "UnitTestsLogger.hpp"
#include <stdarg.h>

using namespace StdLib;

#if defined(PLATFORM_ANDROID)
    void OnLogMessage(const char *message);
    void PopLastMessage(unsigned int length);
#endif

bool UnitTestsLogger::IsMessagePoppingSupported()
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_ANDROID)
    return true;
#else
    return false;
#endif
}

void UnitTestsLogger::PopLastMessage()
{
#if defined(PLATFORM_WINDOWS)
    static NativeConsole console;
    if (!console)
    {
		console.Open(true);
		if (!console)
		{
			console.Open(false);
		}
    }
	console.CursorPosition(0, std::nullopt);
    static char message[4096];
    if (!message[0])
    {
        std::fill(message, message + CountOf(message), ' ');
    }
	console.WriteASCII({message, std::min<ui32>((ui32)CountOf(message), _lastMessageLength)});
	console.CursorPosition(0, std::nullopt);
#elif defined(PLATFORM_ANDROID)
    ::PopLastMessage(_lastMessageLength);
#else
    HARDBREAK;
#endif
}

void UnitTestsLogger::Message(PRINTF_VERIFY_FRONT const char *fmt, ...)
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
    vfprintf(stderr, fmt, args);
#endif
    va_end(args);
}
