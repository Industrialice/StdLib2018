#pragma once

#include "AssignListenerId.hpp"

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
		void Message(LogLevels::LogLevel level, const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(4, 5);
		void Info(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Error(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Warning(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Critical(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Debug(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Attention(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Other(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		
		void Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, va_list args);
		void Error(const MetaType &meta, const char *format, va_list args);
		void Warning(const MetaType &meta, const char *format, va_list args);
		void Critical(const MetaType &meta, const char *format, va_list args);
		void Debug(const MetaType &meta, const char *format, va_list args);
		void Attention(const MetaType &meta, const char *format, va_list args);
		void Info(const MetaType &meta, const char *format, va_list args);
		void Other(const MetaType &meta, const char *format, va_list args);
	};
	template <bool IsThreadSafe> struct _LoggerMessageMethod<void, IsThreadSafe>
	{
		void Message(LogLevels::LogLevel level, PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(3, 4);
		void Info(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Error(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Warning(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Critical(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Debug(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Attention(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);
		void Other(PRINTF_VERIFY_FRONT const char *format, ...) PRINTF_VERIFY_BACK(2, 3);

		void Message(LogLevels::LogLevel level, const char *format, va_list args);
		void Info(const char *format, va_list args);
		void Error(const char *format, va_list args);
		void Warning(const char *format, va_list args);
		void Critical(const char *format, va_list args);
		void Debug(const char *format, va_list args);
		void Attention(const char *format, va_list args);
		void Other(const char *format, va_list args);
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

		// note that if you have listeners in different threads
		// moving or destroying the logger will be a race condition
		Logger();
		Logger(Logger &&source) noexcept;
		Logger &operator = (Logger &&source) noexcept;

		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Message;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Info;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Error;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Warning;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Critical;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Debug;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Attention;
		using _LoggerMessageMethod<MetaType, IsThreadSafe>::Other;
		[[nodiscard]] ListenerHandle OnMessage(const ListenerCallbackType &listener, LogLevels::LogLevel levelMask = LogLevels::_All);
		void RemoveListener(ListenerHandle &handle);
		void IsEnabled(bool isEnabled);
		[[nodiscard]] bool IsEnabled() const;

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

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(level, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Info(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Info, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Error(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Error, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Warning(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Warning, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Critical(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Critical, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Debug(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Debug, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Attention(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Attention, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Other(const MetaType &meta, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Other, &meta, format, args);
		va_end(args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(level, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Info(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Info, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Error(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Error, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Warning(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Warning, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Critical(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Critical, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Debug(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Debug, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Attention(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Attention, &meta, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> inline void _LoggerMessageMethod<MetaType, IsThreadSafe>::Other(const MetaType &meta, const char *format, va_list args)
	{
		static_cast<Logger<MetaType, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Other, &meta, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Message(LogLevels::LogLevel level, PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(level, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Info(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Info, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Error(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Error, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Warning(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Warning, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Critical(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Critical, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Debug(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Debug, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Attention(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Attention, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Other(PRINTF_VERIFY_FRONT const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Other, nullptr, format, args);
		va_end(args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Message(LogLevels::LogLevel level, const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(level, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Info(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Info, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Error(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Error, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Warning(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Warning, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Critical(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Critical, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Debug(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Debug, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Attention(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Attention, nullptr, format, args);
	}

	template <bool IsThreadSafe> inline void _LoggerMessageMethod<void, IsThreadSafe>::Other(const char *format, va_list args)
	{
		static_cast<Logger<void, IsThreadSafe> *>(this)->MessageImpl(LogLevels::Other, nullptr, format, args);
	}

	template <typename MetaType, bool IsThreadSafe> void Logger<MetaType, IsThreadSafe>::RemoveListener(LoggerLocation *instance, void *handle)
	{
		instance->logger->RemoveListener(*static_cast<ListenerHandle *>(handle));
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
			printed = vsnprintf(targetBuffer, static_cast<uiw>(printed) + 1, format, args);
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
					listener.callback(level, std::string_view(targetBuffer, printed), *static_cast<const MetaType *>(meta));
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