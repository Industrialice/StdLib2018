#pragma once

#include "CoreHeader.hpp"

namespace StdLib
{
    template <typename T> struct ArgConverter
    {
        typedef T stored;

        static T &FromStored(stored &arg)
        {
            return arg;
        }

        template <typename X> static T ToStored(X &&arg)
        {
            return std::forward<T>(arg);
        }

        /*template <typename = std::enable_if_t<std::is_arithmetic<T>::value == false>> static T &ToStored(T &arg)
        {
            return arg;
        }

        template <typename = std::enable_if_t<std::is_arithmetic<T>::value == true>> static T ToStored(T arg)
        {
            return arg;
        }*/
    };

    /*template < typename T > struct ArgConverter < T & >
    {
        typedef T stored;

        static T &FromStored( stored &arg )
        {
            static_assert( false, "modifiable references are forbidden" );
        }

        static T &ToStored( T &arg )
        {
            static_assert( false, "modifiable references are forbidden" );
        }
    };

    template < typename T > struct ArgConverter < const T & >
    {
        typedef T stored;

        static const T &FromStored( const stored &arg )
        {
            return arg;
        }

        static const T &ToStored( const T &arg )
        {
            return arg;
        }
    };*/

    template <typename T> struct ArgConverter<T *>
    {
        using stored = T *;

        static T *FromStored(stored arg)
        {
            return arg;
        }

        static T *ToStored(T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<const T *>
    {
        using stored = const T *;

        static const T *FromStored(stored arg)
        {
            return arg;
        }

        static const T *ToStored(const T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<T[]>
    {
        using stored = T *;

        static T *FromStored(stored *arg)
        {
            return arg;
        }

        static T *ToStored(T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<const T[]>
    {
        using stored = const T *;

        static const T *FromStored(stored arg)
        {
            return arg;
        }

        static const T *ToStored(const T *arg)
        {
            return arg;
        }
    };

    template <typename T, size_t size> struct ArgConverter<T[size]>
    {
        using stored = T *;

        static T *FromStored(stored *arg)
        {
            return arg;
        }

        static T *ToStored(T *arg)
        {
            return arg;
        }
    };

    template <typename T, size_t size> struct ArgConverter<const T[size]>
    {
        using stored = const T *;

        static const T *FromStored(stored arg)
        {
            return arg;
        }

        static const T *ToStored(const T *arg)
        {
            return arg;
        }
    };

    template <typename T> struct ArgConverter<T &&>
    {
        using stored = T;

        static T &&FromStored(stored &arg)
        {
            return std::move(arg);
        }

        static T &ToStored(T &&arg)
        {
            return std::move(arg);
        }
    };

    template <int...> struct seq
    {};

    template <int N, int... S> struct gens : gens<N - 1, N - 1, S...>
    {};

    template <int... S> struct gens<0, S...>
    {
        typedef seq<S...> type;
    };

    template <typename T> struct GetCallablePointer;

    template <typename T> struct GetCallablePointer<T *>
    {
        static T *Get(T *caller)
        {
            return caller;
        }
    };

    template <typename T> struct GetCallablePointer<const T *>
    {
        static const T *Get(const T *caller)
        {
            return caller;
        }
    };

    template <typename T> struct GetCallablePointer<T * const>
    {
        static T *Get(T *caller)
        {
            return caller;
        }
    };

    template <typename T> struct GetCallablePointer<const T * const>
    {
        static const T *Get(const T *caller)
        {
            return caller;
        }
    };

    template <typename T> struct GetCallablePointer<T &>
    {
        static T *Get(T &caller)
        {
            return &caller;
        }
    };

    template <typename T> struct GetCallablePointer<const T &>
    {
        static const T *Get(const T &caller)
        {
            return &caller;
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

    template <typename Base, typename Caller, auto methodToCall, typename... MessageArgs> struct MessageDelegate : public MessageBase<Base>
    {
        Caller caller;
        std::tuple<typename ArgConverter<MessageArgs>::stored...> args;

		template <int... S> static void callFunc(i32 action, MessageBase<Base> *object, seq<S...>)
		{
			MessageDelegate *helper = (MessageDelegate *)object;
			if (action & 1)
			{
				(GetCallablePointer<Caller>::Get(helper->caller)->*methodToCall)(ArgConverter<MessageArgs>::FromStored(std::get<S>(helper->args))...);
			}
			if (action & 2)
			{
				helper->~MessageDelegate();
			}
        }

        static void CallFunc(typename MessageBase<Base>::Action action, MessageBase<Base> *object)
        {
            callFunc((i32)action, object, typename gens<sizeof...(MessageArgs)>::type());
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

        template <int... S> static void callFunc(i32 action, MessageBase<Base> *object, seq<S...>)
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
            callFunc((i32)action, object, typename gens<sizeof...(MessageArgs)>::type());
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

        template <int... S> static void callFunc(i32 action, MessageBase<Base> *object, seq<S...>)
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
            callFunc((i32)action, object, typename gens<sizeof...(MessageArgs)>::type());
        }

        template <typename... TMessageArgs> MessageFuncPointer(FuncType func, TMessageArgs &&... args) :
            func(func),
            args(ArgConverter<TMessageArgs>::ToStored(std::forward<TMessageArgs>(args))...)
        {
            this->_execute = &CallFunc;
        }
    };
}