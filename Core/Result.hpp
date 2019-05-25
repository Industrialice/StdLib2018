#pragma once

#include "Error.hpp"

namespace StdLib
{
    ERROR_CODE_DEFINITION(0, AlreadyUnwrapped);
    [[nodiscard]] inline Error<> AlreadyUnwrapped() { return Error<>(_ErrorCodes::AlreadyUnwrapped(), "Result", nullptr); }

    template <typename T, typename ErrorAttachmentType = void, typename ErrorDescriptionType = const char *> class Result
    {
        using ErrorType = Error<ErrorAttachmentType, ErrorDescriptionType>;

        union ValueStore
        {
            ~ValueStore()
            {}

            ValueStore()
            {}

			char empty;
			T value;
        };
		ValueStore _value{};
		ErrorType _error{};

    public:
        ~Result()
        {
            if (_error.IsOk())
            {
                _value.value.~T();
            }
        }

        Result(const T &value) : _error(ErrorType::FromDefaultError(DefaultError::Ok()))
        {
            _value.value = value;
        }

        Result(T &&value) : _error(ErrorType::FromDefaultError(DefaultError::Ok()))
        {
            _value.value = std::move(value);
        }

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
            return std::move(_value.value);
        }

        template <typename E> [[nodiscard]] T UnwrapOrGet(E &&defaultValue)
        {
            ASSUME(_error != AlreadyUnwrapped());
            if (_error.IsOk())
            {
                _error = ErrorType::FromDefaultError(AlreadyUnwrapped());
                return std::move(_value.value);
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