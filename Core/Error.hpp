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
        Error(const _ErrorCodes::ErrorCodeBase &code, const char *errorClass, const char *description) : _code(code), _errorClass(errorClass), _description(description)
        {
            if (!_errorClass)
            {
                _errorClass = "Default";
            }
            if (!_description)
            {
                _description = code.Label();
            }
        }

        [[nodiscard]] bool IsOk() const
        {
            return _code.Code() == 0;
        }

        [[nodiscard]] std::pair<ui32, const char *> Code() const
        {
            return {_code.Code(), _code.Label()};
        }

        [[nodiscard]] const char *ErrorClass() const
        {
            return _errorClass;
        }

        [[nodiscard]] const char *Description() const
        {
            return _description;
        }

        [[nodiscard]] bool operator == (const Error &other) const
        {
            return _code.Code() == other._code.Code() && _code.Label() == other._code.Label() && !strcmp(_errorClass, other._errorClass);
        }

        [[nodiscard]] bool operator != (const Error &other) const
        {
            return !(this->operator == (other));
        }

        [[nodiscard]] static Error FromDefaultError(const Error<> &source)
        {
            return source;
        }

        [[nodiscard]] explicit operator bool() const
        {
            return _code.Code() != 0;
        }
    };

    template <typename AttachmentType> class Error<AttachmentType, const char *> : public Error<>
    {
        AttachmentType _attachment{};

    public:
        Error(const Error<> &error, const AttachmentType &attachment) : Error<>(error), _attachment(attachment)
        {}

        Error(const Error<> &error, AttachmentType &&attachment) : Error<>(error), _attachment(std::move(attachment))
        {}

        [[nodiscard]] const AttachmentType &Attachment() const
        {
            return _attachment;
        }

        [[nodiscard]] static Error FromDefaultError(const Error<> &source)
        {
            return {source, {}};
        }
    };

    template <typename DescriptionType> class Error<void, DescriptionType> : public Error<>
    {
        DescriptionType _description;

    public:
        Error(const Error<> &error, const DescriptionType &description) : Error<>(error), _description(description)
        {}

        Error(const Error<> &error, DescriptionType &&description) : Error<>(error), _description(std::move(description))
        {}

        [[nodiscard]] const DescriptionType &Description() const
        {
            return _description;
        }

        [[nodiscard]] static Error FromDefaultError(const Error<> &source)
        {
            return {source, {}};
        }
    };

    template <typename AttachmentType, typename DescriptionType> class Error : public Error<AttachmentType, const char *>
    {
        DescriptionType _description;

    public:
        Error(const Error<> &error, const AttachmentType &attachment, const DescriptionType &description) : Error<AttachmentType>(error, attachment), _description(description)
        {}

        Error(const Error<> &error, const AttachmentType &attachment, DescriptionType &&description) : Error<AttachmentType>(error, attachment), _description(std::move(description))
        {}

        Error(const Error<> &error, AttachmentType &&attachment, const DescriptionType &description) : Error<AttachmentType>(error, std::move(attachment)), _description(description)
        {}

        Error(const Error<> &error, AttachmentType &&attachment, DescriptionType &&description) : Error<AttachmentType>(error, std::move(attachment)), _description(std::move(description))
        {}

        [[nodiscard]] const DescriptionType &Description() const
        {
            return _description;
        }

        [[nodiscard]] static Error FromDefaultError(const Error<> &source)
        {
            return {source, {}, {}};
        }
    };

    namespace DefaultError
    {
        [[nodiscard]] inline Error<> Ok(const char *description = nullptr) { return Error<>(_ErrorCodes::Ok(), nullptr, description); }
        [[nodiscard]] inline Error<> UnknownError(const char *description = nullptr) { return Error<>(_ErrorCodes::UnknownError(), nullptr, description); }
        [[nodiscard]] inline Error<> InvalidArgument(const char *description = nullptr) { return Error<>(_ErrorCodes::InvalidArgument(), nullptr, description); }
        [[nodiscard]] inline Error<> OutOfMemory(const char *description = nullptr) { return Error<>(_ErrorCodes::OutOfMemory(), nullptr, description); }
        [[nodiscard]] inline Error<> AlreadyExists(const char *description = nullptr) { return Error<>(_ErrorCodes::AlreadyExists(), nullptr, description); }
        [[nodiscard]] inline Error<> NotFound(const char *description = nullptr) { return Error<>(_ErrorCodes::NotFound(), nullptr, description); }
        [[nodiscard]] inline Error<> AccessDenied(const char *description = nullptr) { return Error<>(_ErrorCodes::AccessDenied(), nullptr, description); }
        [[nodiscard]] inline Error<> Unsupported(const char *description = nullptr) { return Error<>(_ErrorCodes::Unsupported(), nullptr, description); }
        [[nodiscard]] inline Error<> NotImplemented(const char *description = nullptr) { return Error<>(_ErrorCodes::NotImplemented(), nullptr, description); }
        [[nodiscard]] inline Error<> Obsolete(const char *description = nullptr) { return Error<>(_ErrorCodes::Obsolete(), nullptr, description); }
        [[nodiscard]] inline Error<> Interrupted(const char *description = nullptr) { return Error<>(_ErrorCodes::Interrupted(), nullptr, description); }
        [[nodiscard]] inline Error<> Cancelled(const char *description = nullptr) { return Error<>(_ErrorCodes::Cancelled(), nullptr, description); }
        [[nodiscard]] inline Error<> UnknownFormat(const char *description = nullptr) { return Error<>(_ErrorCodes::UnknownFormat(), nullptr, description); }
        [[nodiscard]] inline Error<> Corrupted(const char *description = nullptr) { return Error<>(_ErrorCodes::Corrupted(), nullptr, description); }
        [[nodiscard]] inline Error<> NotReady(const char *description = nullptr) { return Error<>(_ErrorCodes::NotReady(), nullptr, description); }
        [[nodiscard]] inline Error<> Busy(const char *description = nullptr) { return Error<>(_ErrorCodes::Busy(), nullptr, description); }
        [[nodiscard]] inline Error<> Timeouted(const char *description = nullptr) { return Error<>(_ErrorCodes::Timeouted(), nullptr, description); }
    }
}