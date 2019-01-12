#pragma once

class Logger
{
    static inline StdLib::ui32 _lastMessageLength;

public:
    static bool IsMessagePoppingSupported();
    static void PopLastMessage();
    static void Message(const char *fmt, ...);
};