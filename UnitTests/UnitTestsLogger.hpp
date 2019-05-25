#pragma once

class UnitTestsLogger
{
    static inline StdLib::ui32 _lastMessageLength;

public:
    static bool IsMessagePoppingSupported();
    static void PopLastMessage();
    static void Message(PRINTF_VERIFY_FRONT const char *fmt, ...) PRINTF_VERIFY_BACK(1, 2);
};