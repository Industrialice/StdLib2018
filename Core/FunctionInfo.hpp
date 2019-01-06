#pragma once

// TODO: lambda support?

namespace StdLib::FunctionInfo
{
    template <typename T>
    struct Info {};

    template <typename T>
    struct Info<T *> {};

    template <typename T, typename Class>
    struct Info<T(Class::*)> {};

    template <typename Result, typename... Args>
    struct Info<Result(Args...)>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(*)(Args...);
        using parentClass = void;
    };

    template <typename Result, typename... Args>
    struct Info<Result(*)(Args...)>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(*)(Args...);
        using parentClass = void;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...)>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...);
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) const>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) const;
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) volatile>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) volatile;
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) volatile const>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) volatile const;
        using parentClass = Class;
    };

    template <typename Result, typename... Args>
    struct Info<Result(Args...) noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(*)(Args...) noexcept;
        using parentClass = void;
    };

    template <typename Result, typename... Args>
    struct Info<Result(*)(Args...) noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(*)(Args...) noexcept;
        using parentClass = void;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) noexcept;
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) const noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) const noexcept;
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) volatile noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) volatile noexcept;
        using parentClass = Class;
    };

    template <typename Class, typename Result, typename... Args>
    struct Info<Result(Class::*)(Args...) volatile const noexcept>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(Class::*)(Args...) volatile const noexcept;
        using parentClass = Class;
    };

    template <typename Result, typename... Args>
    struct Info<std::function< Result(Args...) >>
    {
        using args = std::tuple<Args...>;
        using result = Result;
        using type = Result(*)(Args...);
        using parentClass = void;
    };
}