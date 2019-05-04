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
		void Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, va_list args);
    };
    template <bool IsThreadSafe> struct _LoggerMessageMethod<void, IsThreadSafe>
    {
		void Message(LogLevels::LogLevel level, const char *format, ...);
		void Message(LogLevels::LogLevel level, const char *format, va_list args);
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

        static constexpr uiw logBufferSize = 4096;

		using _LoggerThreadSafeData<IsThreadSafe>::_isEnabled;
        std::vector<MessageListener> _listeners{};
        ui32 _currentId = 0;
        std::shared_ptr<LoggerLocation> _loggerLocation{};
        char _logBuffer[logBufferSize];
    };

	////////////////////
	// IMPLEMENTATION //
	////////////////////

	template<typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		((Logger<MetaType, IsThreadSafe> *)this)->MessageImpl(level, &meta, format, args);
		va_end(args);
	}

	template<typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, va_list args)
	{
		((Logger<MetaType, IsThreadSafe> *)this)->MessageImpl(level, &meta, format, args);
	}

	template<bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Message(LogLevels::LogLevel level, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		((Logger<void, IsThreadSafe> *)this)->MessageImpl(level, nullptr, format, args);
		va_end(args);
	}

	template<bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Message(LogLevels::LogLevel level, const char *format, va_list args)
	{
		((Logger<void, IsThreadSafe> *)this)->MessageImpl(level, nullptr, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> void Logger<MetaType, IsThreadSafe>::RemoveListener(LoggerLocation *instance, void *handle)
	{
		instance->logger->RemoveListener(*(ListenerHandle *)handle);
	}

	template <typename MetaType, bool IsThreadSafe> Logger<MetaType, IsThreadSafe>::Logger()
	{
		_loggerLocation = std::make_shared<LoggerLocation>(*this);
	}

	template <typename MetaType, bool IsThreadSafe> Logger<MetaType, IsThreadSafe>::Logger(Logger &&source) noexcept
	{
		_listeners = move(source._listeners);
		_currentId = source._currentId;
		_isEnabled = source._isEnabled;
		_loggerLocation = move(source._loggerLocation);
		_loggerLocation->logger = this;
	}

	template <typename MetaType, bool IsThreadSafe> Logger<MetaType, IsThreadSafe> &Logger<MetaType, IsThreadSafe>::operator = (Logger &&source) noexcept
	{
		_listeners = move(source._listeners);
		_currentId = source._currentId;
		_isEnabled = source._isEnabled;
		_loggerLocation = move(source._loggerLocation);
		_loggerLocation->logger = this;
		return *this;
	}

	template <typename MetaType, bool IsThreadSafe> void Logger<MetaType, IsThreadSafe>::MessageImpl(LogLevels::LogLevel level, const void *meta, const char *format, va_list args)
	{
		if (_isEnabled == false)
		{
			return;
		}

		std::optional<std::scoped_lock<std::mutex>> scopeLock;
		if constexpr (IsThreadSafe)
		{
			scopeLock.emplace(this->_mutex);
		}

		if (_listeners.empty())
		{
			return;
		}

		std::unique_ptr<char[]> tempBuffer;
		char *targetBuffer = _logBuffer;

		int printed = vsnprintf(_logBuffer, logBufferSize, format, args);
		if (printed <= 0)
		{
			SOFTBREAK;
			return;
		}
		if (printed >= logBufferSize) // buffer wasn't large enough
		{
			tempBuffer = std::make_unique<char[]>(printed + 1);
			targetBuffer = tempBuffer.get();
			printed = vsnprintf(targetBuffer, (uiw)printed + 1, format, args);
		}

		for (auto it = _listeners.rbegin(); it != _listeners.rend(); ++it)
		{
			const auto &listener = *it;
			if (listener.levelMask.Contains(level))
			{
				if constexpr (std::is_same_v<MetaType, void>)
				{
					listener.callback(level, std::string_view(targetBuffer, printed));
				}
				else
				{
					listener.callback(level, std::string_view(targetBuffer, printed), *(MetaType *)meta);
				}
			}
		}
	}

	template <typename MetaType, bool IsThreadSafe> auto Logger<MetaType, IsThreadSafe>::OnMessage(const ListenerCallbackType &listener, LogLevels::LogLevel levelMask) -> ListenerHandle
	{
		std::optional<std::scoped_lock<std::mutex>> scopeLock;
		if constexpr (IsThreadSafe)
		{
			scopeLock.emplace(this->_mutex);
		}

		if (levelMask == LogLevels::_None) // not an error, but probably an unexpected case
		{
			SOFTBREAK;
			return {};
		}

		ui32 id = AssignId<MessageListener, ui32, &MessageListener::id>(_currentId, _listeners.begin(), _listeners.end());
		_listeners.push_back({listener, levelMask, id});
		return {_loggerLocation, id};
	}

	template <typename MetaType, bool IsThreadSafe> void Logger<MetaType, IsThreadSafe>::RemoveListener(ListenerHandle &handle)
	{
		std::optional<std::scoped_lock<std::mutex>> scopeLock;
		if constexpr (IsThreadSafe)
		{
			scopeLock.emplace(this->_mutex);
		}

		std::weak_ptr<typename ListenerHandle::ownerType> currentOwner{};
		handle.Owner().swap(currentOwner);
		if (currentOwner.expired())
		{
			return;
		}

		ASSUME(Funcs::AreSharedPointersEqual(currentOwner, _loggerLocation));

		for (auto it = _listeners.begin(); ; ++it)
		{
			ASSUME(it != _listeners.end());
			if (it->id == handle.Id())
			{
				_listeners.erase(it);
				break;
			}
		}
	}

	template <typename MetaType, bool IsThreadSafe> void Logger<MetaType, IsThreadSafe>::IsEnabled(bool isEnabled)
	{
		_isEnabled = isEnabled;
	}

	template <typename MetaType, bool IsThreadSafe> bool Logger<MetaType, IsThreadSafe>::IsEnabled() const
	{
		return _isEnabled;
	}
}