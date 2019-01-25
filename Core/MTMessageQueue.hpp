#pragma once

#include "Message.hpp"
#include <mutex>
#include <condition_variable>

namespace StdLib
{
    // it's a simple proto
    class MTMessageQueue
    {
		struct MessageWithNext
		{
			MessageBase<MessageWithNext> *nextMessage = nullptr;
		};

		using Message = MessageBase<MessageWithNext>;

		Message *_firstMessage{}, *_lastMessage{};
        std::mutex _mutex{};
        std::condition_variable _newWorkNotifier{};

    public:
        template <typename MethodType, MethodType method, typename Caller, typename... VArgs> void AddDelegate(Caller &&caller, VArgs &&... args)
        {
            using messageType = MessageDelegate<MessageWithNext, Caller, MethodType, method, typename std::remove_reference<VArgs>::type...>;
            auto newMessage = new messageType(std::forward<Caller>(caller), std::forward<typename std::remove_reference<VArgs>::type>(args)...);
            std::scoped_lock lock{_mutex};
            NewMessage(newMessage);
            _newWorkNotifier.notify_all();
        }

        template <typename FuncType, FuncType *func, typename... VArgs> void AddFunctionInline(VArgs &&... args)
        {
            using messageType = MessageFuncInline<MessageWithNext, FuncType, func, typename std::remove_reference<VArgs>::type...>;
            auto newMessage = new messageType(std::forward<typename std::remove_reference<VArgs>::type>(args)...);
            std::scoped_lock lock{_mutex};
            NewMessage(newMessage);
            _newWorkNotifier.notify_all();
        }

        template <typename FuncType, typename... VArgs> void AddFunctionPointer(FuncType func, VArgs &&... args)
        {
            using messageType = MessageFuncPointer<MessageWithNext, FuncType, typename std::remove_reference<VArgs>::type...>;
            auto newMessage = new messageType(func, std::forward<typename std::remove_reference<VArgs>::type>(args)...);
            std::scoped_lock lock{_mutex};
            NewMessage(newMessage);
            _newWorkNotifier.notify_all();
        }

        void ExecWait()
        {
            std::unique_lock cvLock{_mutex};
            _newWorkNotifier.wait(cvLock, [this] { return _firstMessage != nullptr; });

			Message *message = _firstMessage;
            _firstMessage = _firstMessage->nextMessage;
            if (_firstMessage == nullptr)
            {
                _lastMessage = nullptr;
            }

            cvLock.unlock();

			message->Execute(Message::Action::ProcessAndDestroy);
            delete message;
        }

        bool ExecNoWait() // returns true if something had been exectuted
        {
			Message *message;

            {
                std::scoped_lock scopeLock{_mutex};

                if (_firstMessage == nullptr)
                {
                    return false;
                }

                message = _firstMessage;
                _firstMessage = _firstMessage->nextMessage;
                if (_firstMessage == nullptr)
                {
                    _lastMessage = nullptr;
                }
            }

			message->Execute(Message::Action::ProcessAndDestroy);
            delete message;

            return true;
        }

		void Clear()
		{
			std::scoped_lock scopeLock{_mutex};

			Message *curMessage = _firstMessage;

			while (curMessage)
			{
                Message *nextMessage = curMessage->nextMessage;
				curMessage->Execute(Message::Action::Destroy);
				curMessage = nextMessage;
			}
		}

    private:
        void NewMessage(Message *message)
        {
            if (_lastMessage)
            {
                _lastMessage->nextMessage = message;
            }
            else
            {
                _firstMessage = message;
            }
            _lastMessage = message;
        }
    };
}