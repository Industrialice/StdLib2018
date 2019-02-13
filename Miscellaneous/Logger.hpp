#pragma once

#include "MiscellaneousCoreTypes.hpp"
#include "AssignListenerId.hpp"
#include <ListenerHandle.hpp>
#include <atomic>
#include <mutex>
#include <cstdarg>

namespace StdLib
{
    struct LogLevels
    {
        static constexpr struct LogLevel : EnumCombinable<LogLevel, ui32, true>
        {} _None = LogLevel::Create(0),
            Info = LogLevel::Create(1 << 0),
            Error = LogLevel::Create(1 << 1),
            Warning = LogLevel::Create(1 << 2),
            Critical = LogLevel::Create(1 << 3),
            Debug = LogLevel::Create(1 << 4),
            Attention = LogLevel::Create(1 << 5),
            Other = LogLevel::Create(1 << 6),
            _All = LogLevel::Create(ui32_max);
    };

	template <bool IsThreadSafe> struct _LoggerThreadSafeData
	{
		bool _isEnabled{true};
	};
	template <> struct _LoggerThreadSafeData<true>
	{
		std::atomic<bool> _isEnabled{true};
		std::mutex _mutex{};
	};

    template <typename MetaType, bool IsThreadSafe> struct _LoggerMessageMethod
    {
        void Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, ...);
    };
    template <bool IsThreadSafe> struct _LoggerMessageMethod<void, IsThreadSafe>
    {
        void Message(LogLevels::LogLevel level, const char *format, ...);
    };

	template <typename MetaType> struct _LoggerCallbackType
	{
		using type = std::function<void(LogLevels::LogLevel level, std::string_view message, const MetaType &meta)>;
	};
	template <> struct _LoggerCallbackType<void>
	{
		using type = std::function<void(LogLevels::LogLevel level, std::string_view message)>;
	};

    template <typename MetaType = void, bool IsThreadSafe = false> class Logger : public _LoggerThreadSafeData<IsThreadSafe>, public _LoggerMessageMethod<MetaType, IsThreadSafe>
    {
        friend _LoggerMessageMethod<MetaType, IsThreadSafe>;

        struct LoggerLocation;
        static void RemoveListener(LoggerLocation *instance, void *handle);

        struct LoggerLocation
        {
            Logger *logger;

            using ListenerHandle = TListenerHandle<LoggerLocation, RemoveListener, ui32>;

            LoggerLocation(Logger &logger) : logger(&logger) {}
        };

    public:
		using ListenerCallbackType = typename _LoggerCallbackType<MetaType>::type;
        using ListenerHandle = typename LoggerLocation::ListenerHandle;

        // note that if you have listeners in different threads,
        // moving or destroying the logger will be a race condition
        Logger();
        Logger(Logger &&source) noexcept;
        Logger &operator = (Logger &&source) noexcept;

        using _LoggerMessageMethod<MetaType, IsThreadSafe>::Message;
        ListenerHandle OnMessage(const ListenerCallbackType &listener, LogLevels::LogLevel levelMask = LogLevels::_All);
        void RemoveListener(ListenerHandle &handle);
        void IsEnabled(bool isEnabled);
        bool IsEnabled() const;

    private:        
        void MessageImpl(LogLevels::LogLevel level, const void *meta, const char *format, va_list args);

        struct MessageListener
        {
            ListenerCallbackType callback{};
            LogLevels::LogLevel levelMask{};
            ui32 id{};
        };

        static constexpr uiw logBufferSize = 65536;

		using _LoggerThreadSafeData<IsThreadSafe>::_isEnabled;
        std::vector<MessageListener> _listeners{};
        ui32 _currentId = 0;
        std::shared_ptr<LoggerLocation> _loggerLocation{};
        char _logBuffer[logBufferSize];
    };

	#include "LoggerImpl.hpp"
}