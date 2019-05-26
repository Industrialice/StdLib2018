#pragma once

#include "Error.hpp"

namespace StdLib
{
    ERROR_CODE_DEFINITION(0, AlreadyUnwrapped);
    [[nodiscard]] inline Error<> AlreadyUnwrapped() { return Error<>(_ErrorCodes::AlreadyUnwrapped(), "Result", nullptr); }

    template <typename T, typename ErrorAttachmentType = void, typename ErrorDescriptionType = const char *> class Result
    {
        using ErrorType = Error<ErrorAttachmentType, ErrorDescriptionType>;

		ErrorType _error{};
		std::variant<std::monostate, T> _value{};

    public:
		Result(Result &&source) noexcept : _error(std::move(source._error)), _value(std::move(source._value))
		{
			ASSUME(this != &source);
		}

		Result &operator = (Result &&source) noexcept
		{
			ASSUME(this != &source);
			_error = std::move(source._error);
			_value = std::move(source._value);
			return *this;
		}

        Result(const T &value) : _error(ErrorType::FromDefaultError(DefaultError::Ok())), _value(value)
        {}

        Result(T &&value) : _error(ErrorType::FromDefaultError(DefaultError::Ok())), _value(std::move(value))
        {}

        Result(const ErrorType &error) : _error(error)
        {
            ASSUME(!_error.IsOk());
        }

        Result(ErrorType &&error) : _error(std::move(error))
        {
            ASSUME(!_error.IsOk());
        }

        void AssumeOk() const
        {
            ASSUME(_error.IsOk());
        }

        [[nodiscard]] T Unwrap()
        {
            ASSUME(_error != AlreadyUnwrapped());
            ASSUME(_error.IsOk());
            _error = ErrorType::FromDefaultError(AlreadyUnwrapped());
            return std::move(std::get<T>(_value));
        }

        template <typename E> [[nodiscard]] T UnwrapOrGet(E &&defaultValue)
        {
            ASSUME(_error != AlreadyUnwrapped());
            if (_error.IsOk())
            {
                _error = ErrorType::FromDefaultError(AlreadyUnwrapped());
				return std::move(std::get<T>(_value));
            }
            return std::forward(defaultValue);
        }

        [[nodiscard]] bool IsOk() const
        {
            ASSUME(_error != AlreadyUnwrapped());
            return _error.IsOk();
        }

        [[nodiscard]] const ErrorType &GetError() const
        {
            ASSUME(_error != AlreadyUnwrapped());
            return _error;
        }

        [[nodiscard]] explicit operator bool() const
        {
            return _error.IsOk();
        }
    };
}