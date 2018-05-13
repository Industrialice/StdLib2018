#pragma once

namespace StdLib::_ErrorCodes
{
    class ErrorCodeBase
    {
        ui32 _code;
        const char *_label;

    private:
        ErrorCodeBase() = default;

        friend struct Ok;
        friend struct UnknownError;
        friend struct InvalidArgument;
        friend struct OutOfMemory;
        friend struct AlreadyExists;
        friend struct NotFound;
        friend struct AccessDenied;
        friend struct UnsupportedFormat;
        friend struct UnsupportedFeature;
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

    public:
        ErrorCodeBase(ui32 code, const char *label) : _code(CustomCodeStart() + code), _label(label)
        {}

        [[nodiscard]] ui32 Code() const
        {
            return _code;
        }

        [[nodiscard]] const char *Label() const
        {
            return _label;
        }
    };

#define ERROR_DEFINITION(Code, Name) \
    struct Name : public ErrorCodeBase \
    { \
        Name() \
        { \
            _code = Code; \
            _label = TOSTR(Name); \
        } \
    };

    ERROR_DEFINITION(0, Ok)
    ERROR_DEFINITION(1, UnknownError)
    ERROR_DEFINITION(2, InvalidArgument)
    ERROR_DEFINITION(3, OutOfMemory)
    ERROR_DEFINITION(4, AlreadyExists)
    ERROR_DEFINITION(5, NotFound)
    ERROR_DEFINITION(6, AccessDenied)
    ERROR_DEFINITION(7, UnsupportedFormat)
    ERROR_DEFINITION(8, UnsupportedFeature)
    ERROR_DEFINITION(9, NotImplemented)
    ERROR_DEFINITION(10, Obsolete)
    ERROR_DEFINITION(11, Interrupted)
    ERROR_DEFINITION(12, Cancelled)
    ERROR_DEFINITION(13, UnknownFormat)
    ERROR_DEFINITION(14, Corrupted)
    ERROR_DEFINITION(15, NotReady)
    ERROR_DEFINITION(16, Busy)
    ERROR_DEFINITION(17, Timeouted)

#undef ERROR_DEFINITION
}