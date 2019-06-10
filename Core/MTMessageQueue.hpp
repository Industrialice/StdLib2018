#pragma once

#include "Message.hpp"
#include "SpinLock.hpp"

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
        SpinLock _mutex{};
        std::condition_variable_any _newWorkNotifier{};

    public:
        template <auto Method, typename Caller, typename = std::enable_if_t<std::is_member_function_pointer_v<decltype(Method)>>, typename... VArgs> void Add(Caller &&caller, VArgs &&... args)
        {
            using messageType = MessageDelegate<MessageWithNext, Caller, Method, VArgs...>;
            auto newMessage = new messageType(std::forward<Caller>(caller), std::forward<VArgs>(args)...);
            std::scoped_lock lock{_mutex};
            NewMessage(newMessage);
            _newWorkNotifier.notify_all();
        }

        template <auto Func, typename... VArgs> void Add(VArgs &&... args)
        {
            using messageType = MessageFuncInline<MessageWithNext, Func, VArgs...>;
            auto newMessage = new messageType(std::forward<VArgs>(args)...);
            std::scoped_lock lock{_mutex};
            NewMessage(newMessage);
            _newWorkNotifier.notify_all();
        }

        template <typename FuncType, typename... VArgs> void Add(FuncType func, VArgs &&... args)
        {
            using messageType = MessageFuncPointer<MessageWithNext, FuncType, VArgs...>;
            auto newMessage = new messageType(func, std::forward<VArgs>(args)...);
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