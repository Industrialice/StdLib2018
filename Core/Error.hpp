#pragma once

#include "_ErrorCodes.hpp"

namespace StdLib
{
    template <typename AttachmentType = void, typename DescriptionType = const char *> class Error;

    template <> class Error<void, const char *>
    {
        _ErrorCodes::ErrorCodeBase _code;
        const char *_errorClass;
        const char *_description;

    public:
		constexpr Error() : Error(_ErrorCodes::Ok(), nullptr, nullptr)
		{}

        constexpr Error(const _ErrorCodes::ErrorCodeBase &code, const char *errorClass, const char *description) : _code(code), _errorClass(errorClass), _description(description)
        {
            if (!_errorClass)
            {
                _errorClass = "Default";
            }
            if (!_description)
            {
                _description = "";
            }
        }

        [[nodiscard]] constexpr bool IsOk() const
        {
            return _code.Code() == 0;
        }

        [[nodiscard]] constexpr std::pair<ui32, const char *> Code() const
        {
            return {_code.Code(), _code.Label()};
        }

        [[nodiscard]] constexpr const char *ErrorClass() const
        {
            return _errorClass;
        }

        [[nodiscard]] constexpr const char *Description() const
        {
            return _description;
        }

        [[nodiscard]] constexpr bool operator == (const Error &other) const
        {
            bool equals = _code.Code() == other._code.Code() && !strcmp(_errorClass, other._errorClass);
			ASSUME(equals == false || !strcmp(_code.Label(), other._code.Label()));
			return equals;
        }

        [[nodiscard]] constexpr bool operator != (const Error &other) const
        {
            return !(this->operator == (other));
        }

        [[nodiscard]] static constexpr Error FromDefaultError(const Error<> &source)
        {
            return source;
        }

        [[nodiscard]] constexpr explicit operator bool() const
        {
            return _code.Code() != 0;
        }
    };

    template <typename AttachmentType> class Error<AttachmentType, const char *> : public Error<>
    {
        AttachmentType _attachment{};

    public:
		constexpr Error(const Error<> &error, const AttachmentType &attachment) : Error<>(error), _attachment(attachment)
        {}

		constexpr Error(const Error<> &error, AttachmentType &&attachment) : Error<>(error), _attachment(std::move(attachment))
        {}

        [[nodiscard]] constexpr const AttachmentType &Attachment() const
        {
            return _attachment;
        }

        [[nodiscard]] static constexpr Error FromDefaultError(const Error<> &source)
        {
            return {source, {}};
        }
    };

    template <typename DescriptionType> class Error<void, DescriptionType> : public Error<>
    {
        DescriptionType _description;

    public:
		constexpr Error() = default;

		constexpr Error(const Error<> &error, const DescriptionType &description) : Error<>(error), _description(description)
        {}

		constexpr Error(const Error<> &error, DescriptionType &&description) : Error<>(error), _description(std::move(description))
        {}

        [[nodiscard]] constexpr const DescriptionType &Description() const
        {
            return _description;
        }

        [[nodiscard]] static constexpr Error FromDefaultError(const Error<> &source)
        {
            return {source, {}};
        }
    };

    template <typename AttachmentType, typename DescriptionType> class Error : public Error<AttachmentType, const char *>
    {
        DescriptionType _description;

    public:
		constexpr Error() = default;

		constexpr Error(const Error<> &error, const AttachmentType &attachment, const DescriptionType &description) : Error<AttachmentType>(error, attachment), _description(description)
        {}

		constexpr Error(const Error<> &error, const AttachmentType &attachment, DescriptionType &&description) : Error<AttachmentType>(error, attachment), _description(std::move(description))
        {}

		constexpr Error(const Error<> &error, AttachmentType &&attachment, const DescriptionType &description) : Error<AttachmentType>(error, std::move(attachment)), _description(description)
        {}

		constexpr Error(const Error<> &error, AttachmentType &&attachment, DescriptionType &&description) : Error<AttachmentType>(error, std::move(attachment)), _description(std::move(description))
        {}

        [[nodiscard]] constexpr const DescriptionType &Description() const
        {
            return _description;
        }

        [[nodiscard]] static constexpr Error FromDefaultError(const Error<> &source)
        {
            return {source, {}, {}};
        }
    };

    namespace DefaultError
    {
        [[nodiscard]] constexpr inline Error<> Ok(const char *description = nullptr) { return Error<>(_ErrorCodes::Ok(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> UnknownError(const char *description = nullptr) { return Error<>(_ErrorCodes::UnknownError(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> InvalidArgument(const char *description = nullptr) { return Error<>(_ErrorCodes::InvalidArgument(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> OutOfMemory(const char *description = nullptr) { return Error<>(_ErrorCodes::OutOfMemory(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> AlreadyExists(const char *description = nullptr) { return Error<>(_ErrorCodes::AlreadyExists(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> NotFound(const char *description = nullptr) { return Error<>(_ErrorCodes::NotFound(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> AccessDenied(const char *description = nullptr) { return Error<>(_ErrorCodes::AccessDenied(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Unsupported(const char *description = nullptr) { return Error<>(_ErrorCodes::Unsupported(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> NotImplemented(const char *description = nullptr) { return Error<>(_ErrorCodes::NotImplemented(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Obsolete(const char *description = nullptr) { return Error<>(_ErrorCodes::Obsolete(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Interrupted(const char *description = nullptr) { return Error<>(_ErrorCodes::Interrupted(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Cancelled(const char *description = nullptr) { return Error<>(_ErrorCodes::Cancelled(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> UnknownFormat(const char *description = nullptr) { return Error<>(_ErrorCodes::UnknownFormat(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Corrupted(const char *description = nullptr) { return Error<>(_ErrorCodes::Corrupted(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> NotReady(const char *description = nullptr) { return Error<>(_ErrorCodes::NotReady(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Busy(const char *description = nullptr) { return Error<>(_ErrorCodes::Busy(), nullptr, description); }
        [[nodiscard]] constexpr inline Error<> Timeouted(const char *description = nullptr) { return Error<>(_ErrorCodes::Timeouted(), nullptr, description); }
    }
}