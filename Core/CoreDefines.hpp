#pragma once

namespace StdLib::_Private
{
    void SoftBreak(const char *file, i32 line, i32 counter);
}

#ifdef DEBUG
    #define HARDBREAK _RAISE_EXCEPTION
    #define SOFTBREAK _Private::SoftBreak(__FILE__, __LINE__, __COUNTER__)
    #define UNREACHABLE _RAISE_EXCEPTION
    #ifdef PLATFORM_EMSCRIPTEN
        #define ASSUME(condition) do { if (!(condition)) { printf("!!!ASSUMPTION %s FAILED!!!\n", TOSTR(condition)); HARDBREAK; } } while(0)
    #else
        #define ASSUME(condition) do { if (!(condition)) HARDBREAK; } while(0)
    #endif
#else
    #define HARDBREAK _UNREACHABLE
    #define SOFTBREAK
    #define UNREACHABLE _UNREACHABLE
    #define ASSUME(condition) _ASSUME(condition)
#endif

#define NOIMPL HARDBREAK

#define TOSTR(code) #code
#define CONCAT(first, second) first##second

#define ENUM_COMBINABLE(Name, Type, ...) \
    struct Name \
    { \
        enum : Type \
        { \
            __VA_ARGS__ \
        } _value; \
        using _type = decltype(_value); \
        Name() = default; \
        Name(decltype(_value) source) : _value(source) {} \
        Name operator - (_type other) const { return _type(Type(_value) & ~Type(other)); } \
        Name operator - (Name other) const { return _type(Type(_value) & ~Type(other._value)); } \
        Name operator + (_type other) const { return _type(Type(_value) | Type(other)); } \
        Name operator + (Name other) const { return _type(Type(_value) | Type(other._value)); } \
        bool operator && (_type other) const { return _type(Type(_value) & Type(other)) == other; } \
        bool operator && (Name other) const { return _type(Type(_value) & Type(other._value)) == other._value; } \
        Name &operator -= (_type other) { _value = (Name(_value) - Name(other))._value; return *this; } \
        Name &operator -= (Name other) { _value = (Name(_value) - Name(other._value))._value; return *this; } \
        Name &operator += (_type other) { _value = (Name(_value) + Name(other))._value; return *this; } \
        Name &operator += (Name other) { _value = (Name(_value) + Name(other._value))._value; return *this; } \
        explicit operator bool() const { return (Type)_value != 0; } \
        bool operator == (_type other) const { return _value == other; } \
        bool operator == (Name other) const { return _value == other._value; } \
        bool operator != (_type other) const { return _value != other; } \
        bool operator != (Name other) const { return _value != other._value; } \
    };

#define ENUM_COMBINABLE_OPS(Name, Type) \
    inline Name operator - (Name::_type left, Name::_type right) { return Name(Name::_type(Type(left) & ~Type(right))); } \
    inline Name operator + (Name::_type left, Name::_type right) { return Name(Name::_type(Type(left) | Type(right))); } \
    inline bool operator && (Name::_type left, Name::_type right) { return Name(Name::_type(Type(left) & Type(right)))._value == right; }

#define ENUM_COMBINABLE_WITH_OPS(Name, Type, ...) ENUM_COMBINABLE(Name, Type, __VA_ARGS__) ENUM_COMBINABLE_OPS(Name, Type)