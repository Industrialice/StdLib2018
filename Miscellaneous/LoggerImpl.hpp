#include "Logger.hpp"
#pragma once

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

template <typename MetaType, bool IsThreadSafe>
template <typename>
void Logger<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const MetaType &meta, const char *format, ...)
{
	if (_isEnabled == false)
	{
		return;
	}

	optional<std::scoped_lock<std::mutex>> scopeLock;
	if constexpr (IsThreadSafe)
	{
		scopeLock.emplace(this->_mutex);
	}

	if (_listeners.empty())
	{
		return;
	}

	va_list args;
	va_start(args, format);

	uiw printed = 0;
	if (meta.size())
	{
		printed = std::min(meta.size(), logBufferSize - 1);
		memcpy(_logBuffer, meta.data(), printed);
	}

	int variadicPrintResult = vsnprintf(_logBuffer + printed, logBufferSize - printed, format, args);

	va_end(args);

	if (variadicPrintResult <= 0)
	{
		SOFTBREAK;
		return;
	}

	printed += (uiw)variadicPrintResult;

	for (auto it = _listeners.rbegin(); it != _listeners.rend(); ++it)
	{
		const auto &listener = *it;
		if (listener.levelMask.Contains(level))
		{
			listener.callback(level, string_view(_logBuffer, printed));
		}
	}
}

template <typename MetaType, bool IsThreadSafe> NOINLINE void StdLib::Logger<MetaType, IsThreadSafe>::Message(LogLevels::LogLevel level, const char *format, ...)
{
	return void();
}

template <typename MetaType, bool IsThreadSafe> auto Logger<MetaType, IsThreadSafe>::OnMessage(const ListenerCallbackType &listener, LogLevels::LogLevel levelMask) -> ListenerHandle
{
	optional<std::scoped_lock<std::mutex>> scopeLock;
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
	optional<std::scoped_lock<std::mutex>> scopeLock;
	if constexpr (IsThreadSafe)
	{
		scopeLock.emplace(this->_mutex);
	}

	std::weak_ptr<ListenerHandle::ownerType> currentOwner{};
	handle.Owner().swap(currentOwner);
	if (currentOwner.expired())
	{
		return;
	}

	ASSUME(Funcs::AreSharedPointersEqual(handle.Owner(), _loggerLocation));

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
