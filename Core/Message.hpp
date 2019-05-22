#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    template <typename T> struct ArgConverter
    {
        using stored = T;

		template <typename X> [[nodiscard]] static auto FromStored(X &&arg)
        {
            return std::forward<T>(arg);
        }

        template <typename X> [[nodiscard]] static auto ToStored(X &&arg)
        {
            return std::forward<T>(arg);
        }
    };

    template <typename T> struct ArgConverter<T &>
    {
		static constexpr bool isArray = std::is_array_v<T>;
		using stored = std::conditional_t<isArray, std::remove_all_extents_t<T> *, T>;

		[[nodiscard]] static decltype(auto) FromStored(stored &arg)
        {
			if constexpr (isArray)
				return arg;
			else
				return (T &)arg;
        }

		[[nodiscard]] static decltype(auto) ToStored(T &arg)
        {
			if constexpr (isArray)
				return (stored)arg;
			else
				return (stored &)arg;
        }
    };

	template <typename T> struct ArgConverter<const T &>
	{
		static constexpr bool isArray = std::is_array_v<T>;
		using stored = const std::conditional_t<isArray, std::remove_all_extents_t<T> *, T>;

		[[nodiscard]] static decltype(auto) FromStored(stored &arg)
		{
			if constexpr (isArray)
				return arg;
			else
				return (const T &)arg;
		}

		[[nodiscard]] static decltype(auto) ToStored(const T &arg)
		{
			if constexpr (isArray)
				return (stored)arg;
			else
				return (stored &)arg;
		}
	};

    template <typename T> struct ArgConverter<T *>
    {
        using stored = T *;

		[[nodiscard]] static T *FromStored(stored arg)
        {
            return arg;
        }

		[[nodiscard]] static stored ToStored(T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<const T *>
    {
        using stored = const T *;

		[[nodiscard]] static const T *FromStored(stored arg)
        {
            return arg;
        }

		[[nodiscard]] static stored ToStored(const T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<T &&>
    {
        using stored = T;

		[[nodiscard]] static T &&FromStored(stored &arg)
        {
            return std::move(arg);
        }

		[[nodiscard]] static stored ToStored(T &&arg)
        {
            return std::move(arg);
        }
    };

    template <typename Base> class MessageBase : public Base
    {
    public:
		enum class Action : i32 { Process = 1, Destroy, ProcessAndDestroy };

		void Execute(Action action)
		{
			_execute(action, this);
		}

	protected:
		void(*_execute)(Action action, MessageBase *) = 0;
    };

    template <typename Base, typename Caller, auto MethodToCall, typename... MessageArgs> struct MessageDelegate : public MessageBase<Base>
    {
        Caller caller;
        std::tuple<typename ArgConverter<MessageArgs>::stored...> args;

		template <uiw... S> static void callFunc(i32 action, MessageBase<Base> *object, std::index_sequence<S...>)
		{
			MessageDelegate *helper = (MessageDelegate *)object;
			if (action & 1)
			{
				std::invoke(MethodToCall, helper->caller, ArgConverter<MessageArgs>::FromStored(std::get<S>(helper->args))...);
			}
			if (action & 2)
			{
				helper->~MessageDelegate();
			}
        }

        static void CallFunc(typename MessageBase<Base>::Action action, MessageBase<Base> *object)
        {
            callFunc((i32)action, object, std::make_index_sequence<sizeof...(MessageArgs)>());
        }

        template <typename TCaller, typename... TMessageArgs> MessageDelegate(TCaller &&caller, TMessageArgs &&... args) :
            caller(std::forward<TCaller>(caller)),
            args(ArgConverter<TMessageArgs>::ToStored(std::forward<TMessageArgs>(args))...)
        {
			this->_execute = &CallFunc;
        }
    };

    template <typename Base, auto *funcToCall, typename... MessageArgs> struct MessageFuncInline : public MessageBase<Base>
    {
        std::tuple<typename ArgConverter<MessageArgs>::stored...> args;

        template <uiw... S> static void callFunc(i32 action, MessageBase<Base> *object, std::index_sequence<S...>)
        {
            MessageFuncInline *helper = (MessageFuncInline *)object;
			if (action & 1)
			{
				funcToCall(ArgConverter<MessageArgs>::FromStored(std::get<S>(helper->args))...);
			}
			if (action & 2)
			{
				helper->~MessageFuncInline();
			}
        }

        static void CallFunc(typename MessageBase<Base>::Action action, MessageBase<Base> *object)
        {
            callFunc((i32)action, object, std::make_index_sequence<sizeof...(MessageArgs)>());
        }

        template <typename... TMessageArgs> MessageFuncInline(TMessageArgs &&... args) :
            args(ArgConverter<TMessageArgs>::ToStored(std::forward<TMessageArgs>(args))...)
        {
            this->_execute = &CallFunc;
        }
    };

    template <typename Base, typename FuncType, typename... MessageArgs> struct MessageFuncPointer : public MessageBase<Base>
    {
        FuncType func;
        std::tuple<typename ArgConverter<MessageArgs>::stored...> args;

        template <uiw... S> static void callFunc(i32 action, MessageBase<Base> *object, std::index_sequence<S...>)
        {
            MessageFuncPointer *helper = (MessageFuncPointer *)object;
			if (action & 1)
			{
				helper->func(ArgConverter<MessageArgs>::FromStored(std::get<S>(helper->args))...);
			}
			if (action & 2)
			{
				helper->~MessageFuncPointer();
			}
        }

        static void CallFunc(typename MessageBase<Base>::Action action, MessageBase<Base> *object)
        {
            callFunc((i32)action, object, std::make_index_sequence<sizeof...(MessageArgs)>());
        }

        template <typename... TMessageArgs> MessageFuncPointer(FuncType func, TMessageArgs &&... args) :
            func(func),
            args(ArgConverter<TMessageArgs>::ToStored(std::forward<TMessageArgs>(args))...)
        {
            this->_execute = &CallFunc;
        }
    };
}