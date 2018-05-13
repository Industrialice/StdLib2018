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

        [[nodiscard]] bool operator == (const Error &other)
        {
            return _code.Code() == other._code.Code() && _code.Label() == other._code.Label() && !strcmp(_errorClass, other._errorClass);
        }

        [[nodiscard]] bool operator != (const Error &other)
        {
            return !(this->operator == (other));
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
    };

    namespace DefaultError
    {
        [[nodiscard]] inline Error<> Ok() { return Error<>(_ErrorCodes::Ok(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> UnknownError() { return Error<>(_ErrorCodes::UnknownError(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> InvalidArgument() { return Error<>(_ErrorCodes::InvalidArgument(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> OutOfMemory() { return Error<>(_ErrorCodes::OutOfMemory(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> AlreadyExists() { return Error<>(_ErrorCodes::AlreadyExists(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> NotFound() { return Error<>(_ErrorCodes::NotFound(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> AccessDenied() { return Error<>(_ErrorCodes::AccessDenied(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> UnsupportedFormat() { return Error<>(_ErrorCodes::UnsupportedFormat(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> UnsupportedFeature() { return Error<>(_ErrorCodes::UnsupportedFeature(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> NotImplemented() { return Error<>(_ErrorCodes::NotImplemented(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Obsolete() { return Error<>(_ErrorCodes::Obsolete(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Interrupted() { return Error<>(_ErrorCodes::Interrupted(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Cancelled() { return Error<>(_ErrorCodes::Cancelled(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> UnknownFormat() { return Error<>(_ErrorCodes::UnknownFormat(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Corrupted() { return Error<>(_ErrorCodes::Corrupted(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> NotReady() { return Error<>(_ErrorCodes::NotReady(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Busy() { return Error<>(_ErrorCodes::Busy(), nullptr, nullptr); }
        [[nodiscard]] inline Error<> Timeouted() { return Error<>(_ErrorCodes::Timeouted(), nullptr, nullptr); }
    }
}