#pragma once

#include "Error.hpp"

namespace StdLib
{
	#ifdef DEBUG
		ERROR_CODE_DEFINITION(0, AlreadyUnwrapped);
		[[nodiscard]] inline Error<> AlreadyUnwrapped() { return Error<>(_ErrorCodes::AlreadyUnwrapped(), "Result", nullptr); }
	#endif

    template <typename T, typename ErrorAttachmentType = void, typename ErrorDescriptionType = const char *> class Result
    {
        using ErrorType = Error<ErrorAttachmentType, ErrorDescriptionType>;

		ErrorType _error{};
        union ValueStore
        {
			~ValueStore() {}
			ValueStore() {}

			char empty{};
			T value;
		} _value{}; // value is stored only if the error's status is Ok

    public:
        ~Result()
        {
            if (_error.IsOk())
            {
                _value.value.~T();
            }
        }

		Result() = delete;

		Result(Result &&source) noexcept : _error(std::move(source._error))
		{
			ASSUME(this != &source);
			if (_error.IsOk())
			{
				new (&_value.value) T(std::move(source._value.value));
			}
		}

		Result &operator = (Result &&source) noexcept
		{
			ASSUME(this != &source);
			_error = std::move(source._error);
			if (_error.IsOk())
			{
				new (&_value.value) T(std::move(source._value.value));
			}
			return *this;
		}

        Result(const T &value) : _error(ErrorType::FromDefaultError(DefaultError::Ok()))
        {
			new (&_value.value) T(value);
        }

        Result(T &&value) : _error(ErrorType::FromDefaultError(DefaultError::Ok()))
        {
            new (&_value.value) T(std::move(value));
        }

        Result(const ErrorType &error) : _error(error)
        {
            ASSUME(!_error.IsOk());
        }

        Result(ErrorType &&error) : _error(std::move(error))
        {
            ASSUME(!_error.IsOk());
        }

        [[nodiscard]] T Unwrap()
        {
            ASSUME(_error != AlreadyUnwrapped());
            ASSUME(_error.IsOk());
			#ifdef DEBUG
				_error = ErrorType::FromDefaultError(AlreadyUnwrapped());
			#endif
            return std::move(_value.value);
        }

        template <typename E> [[nodiscard]] T UnwrapOrGet(E &&defaultValue)
        {
            ASSUME(_error != AlreadyUnwrapped());
            if (_error.IsOk())
            {
				#ifdef DEBUG
					_error = ErrorType::FromDefaultError(AlreadyUnwrapped());
				#endif
                return std::move(_value.value);
            }
            return std::forward<E>(defaultValue);
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