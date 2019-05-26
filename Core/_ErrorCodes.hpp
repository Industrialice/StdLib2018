#pragma once

#include "CoreDefines.hpp"

namespace StdLib::_ErrorCodes
{
    class ErrorCodeBase
    {
        ui32 _code;
        const char *_label;

    private:
		ErrorCodeBase() = delete;

        friend struct Ok;
        friend struct UnknownError;
        friend struct InvalidArgument;
        friend struct OutOfMemory;
        friend struct AlreadyExists;
        friend struct NotFound;
        friend struct AccessDenied;
        friend struct Unsupported;
        friend struct NotImplemented;
        friend struct Obsolete;
        friend struct Interrupted;
        friend struct Cancelled;
        friend struct UnknownFormat;
        friend struct Corrupted;
        friend struct NotReady;
        friend struct Busy;
        friend struct Timeouted;

    protected:
        [[nodiscard]] static constexpr ui32 CustomCodeStart()
        {
            return 128;
        }

		constexpr ErrorCodeBase(ui32 code, const char *label, std::nullptr_t) : _code(code), _label(label)
		{}

    public:
		constexpr ErrorCodeBase(ui32 code, const char *label) : _code(CustomCodeStart() + code), _label(label)
        {}

        [[nodiscard]] constexpr ui32 Code() const
        {
            return _code;
        }

        [[nodiscard]] constexpr const char *Label() const
        {
            return _label;
        }
    };
}

#define ERROR_CODE_DEFINITION(Code, Name) \
    namespace StdLib::_ErrorCodes \
    { \
        struct Name : public ::StdLib::_ErrorCodes::ErrorCodeBase \
        { \
            constexpr Name() : ErrorCodeBase(Code, TOSTR(Name), std::nullptr_t{}) \
            {} \
        }; \
    }

ERROR_CODE_DEFINITION(0, Ok)
ERROR_CODE_DEFINITION(1, UnknownError)
ERROR_CODE_DEFINITION(2, InvalidArgument)
ERROR_CODE_DEFINITION(3, OutOfMemory)
ERROR_CODE_DEFINITION(4, AlreadyExists)
ERROR_CODE_DEFINITION(5, NotFound)
ERROR_CODE_DEFINITION(6, AccessDenied)
ERROR_CODE_DEFINITION(7, Unsupported)
ERROR_CODE_DEFINITION(8, NotImplemented)
ERROR_CODE_DEFINITION(9, Obsolete)
ERROR_CODE_DEFINITION(10, Interrupted)
ERROR_CODE_DEFINITION(11, Cancelled)
ERROR_CODE_DEFINITION(12, UnknownFormat)
ERROR_CODE_DEFINITION(13, Corrupted)
ERROR_CODE_DEFINITION(14, NotReady)
ERROR_CODE_DEFINITION(15, Busy)
ERROR_CODE_DEFINITION(16, Timeouted)

#undef ERROR_CODE_DEFINITION

#define ERROR_CODE_DEFINITION(Code, Name) \
    namespace _ErrorCodes \
    { \
        struct Name : public ::StdLib::_ErrorCodes::ErrorCodeBase \
        { \
            constexpr Name() : ErrorCodeBase(Code, TOSTR(Name)) \
            {} \
        }; \
    }