#pragma once

#ifdef _MSC_VER

	#ifndef STDLIB_DISABLE_DIAGNOSTIC_OVERRIDES
		#pragma warning(1 : 4062) /* The enumerate has no associated handler in a switch statement, and there is no default label. */
		//#pragma warning(1 : 4265) /* class has virtual functions, but destructor is not virtual */
		#pragma warning(1 : 4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
		#pragma warning(1 : 4263) /* 'function': member function does not override any base class virtual member function */
		#pragma warning(1 : 4264) /* 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden */
		#pragma warning(1 : 4266) /* 'function': no override available for virtual member function from base 'type'; function is hidden */
	#endif

	#define SPACESHIP_SUPPORTED

    #define RSTR __restrict
    #define ALLOCA(count, sizeOfElement) _alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN __declspec(restrict)
    #define ALLOCATORFUNC __declspec(allocator)
    #define NOINLINE __declspec(noinline)
    #define FORCEINLINE __forceinline
	#define UNALIGNEDPTR __unaligned
    #define EMPTY_BASES __declspec(empty_bases)
    #define NOVTABLE __declspec(novtable)
    #define PRINTF_VERIFY_FRONT _In_z_ _Printf_format_string_
    #define PRINTF_VERIFY_BACK(...)
	#define DEPRECATE(foo, msg) __declspec(deprecated(msg)) foo
	#define PRETTY_FUNCTION __FUNCTION__
	#define RETURNS_NONNULL

	#define WARNING_PUSH __pragma(warning(push)) 
	#define WARNING_POP __pragma(warning(pop)) 
	#define WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW __pragma(warning(disable : 4307))
	#define WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED __pragma(warning(disable : 5030))
	#define WARNING_DISABLE_LARGE_BY_VALUE_COPY
	#define WARNING_DISABLE_INCREASES_REQUIRED_ALIGNMENT
	#define WARNING_DISABLE_NONSTANDARD_EXTENSION_USED __pragma(warning(disable : 4200))

    #define _UNREACHABLE __assume(0)

    #define _ASSUME(condition) __assume(condition)

    #include <intrin.h>

	using _IndexOfSignificantBitResultType = unsigned long;
	using _IndexOfSignificantBitInputType = unsigned long;
    #define _MSNZB32(tosearch, result) _BitScanReverse(result, tosearch)
    #define _LSNZB32(tosearch, result) _BitScanForward(result, tosearch)

#ifdef _WIN64
	using _IndexOfSignificantBitInput64Type = unsigned __int64;
    #define _MSNZB64(tosearch, result) _BitScanReverse64(result, tosearch)
    #define _LSNZB64(tosearch, result) _BitScanForward64(result, tosearch)
#endif

	using _ByteSwapInput16 = unsigned short;
	using _ByteSwapInput32 = unsigned long;
	using _ByteSwapInput64 = unsigned __int64;

    #define _BYTESWAP16(value) _byteswap_ushort(value)
    #define _BYTESWAP32(value) _byteswap_ulong(value)
    #define _BYTESWAP64(value) _byteswap_uint64(value)

	using _RotateBitsInput64 = unsigned __int64;
	using _RotateBitsInput32 = unsigned int;
	using _RotateBitsInput16 = unsigned short;
	using _RotateBitsInput8 = unsigned char;

    #define _ROTATE32R(val, shift) _rotr(val, static_cast<int>(shift))
    #define _ROTATE32L(val, shift) _rotl(val, static_cast<int>(shift))
    #define _ROTATE64R(val, shift) _rotr64(val, static_cast<int>(shift))
    #define _ROTATE64L(val, shift) _rotl64(val, static_cast<int>(shift))
    #define _ROTATE16R(val, shift) _rotr16(val, static_cast<unsigned char>(shift))
    #define _ROTATE16L(val, shift) _rotl16(val, static_cast<unsigned char>(shift))
    #define _ROTATE8R(val, shift) _rotr8(val, static_cast<unsigned char>(shift))
    #define _ROTATE8L(val, shift) _rotl8(val, static_cast<unsigned char>(shift))

    // works, but shows worse performance
	//using BitTestInputType = long;
	//#define _BITTEST32(value, position) (_bittest(&value, position) != 0)
	//#if defined(_WIN64) || defined(_M_ARM)
	//	using BitTestInput64Type = long long;
	//	#define _BITTEST64(value, position) (_bittest64(&value, position) != 0)
	//#endif

	//#define _BITTESTANDRESET32(value, position) (_bittestandreset(value, position) != 0)
	//#define _BITTESTANDSET32(value, position) (_bittestandset(value, position) != 0)
	//#if defined(_M_AMD64)
	//	using BitTestInput64Type = long long;
	//	#define _BITTESTANDRESET64(value, position) (_bittestandreset64(value, position) != 0)
	//	#define _BITTESTANDSET64(value, position) (_bittestandset64(value, position) != 0)
	//#endif

#elif defined(__GNUC__) || defined(__clang__) || defined(__EMSCRIPTEN__) || defined(__MINGW32__) || defined(__MINGW32__) || defined(__MINGW64__)

    #define RSTR __restrict__
    #define ALLOCA(count, sizeOfElement) __builtin_alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN /* TODO: find out */
    #define ALLOCATORFUNC /* TODO: find out */
    #define NOINLINE __attribute__((noinline))
    #define FORCEINLINE __attribute__((always_inline)) inline
#ifdef __clang__
	#define UNALIGNEDPTR __unaligned /* It seems there's no alternative for GCC */
#endif
    #define NOVTABLE /* doesn't seem to be supported by neither Clang, nor GCC */
    #define EMPTY_BASES /* TODO: do these compilers have something similar */
    #define PRINTF_VERIFY_FRONT
    #define PRINTF_VERIFY_BACK(stringIndex, argumentIndex) __attribute__((format(printf, stringIndex, argumentIndex))) /* when counting arguments, the first argument has index 1, you should also account for this argument */
	#define DEPRECATE(foo, msg) foo __attribute__((deprecated(msg)))
	#define PRETTY_FUNCTION __PRETTY_FUNCTION__
	#define RETURNS_NONNULL __attribute__((returns_nonnull))

	#ifdef __clang__
		#define WARNING_PUSH _Pragma("clang diagnostic push")
		#define WARNING_POP _Pragma("clang diagnostic pop")
		#define WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW _Pragma("clang diagnostic ignored \"-Winteger-overflow\"")
		#define WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED _Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
		#define WARNING_DISABLE_LARGE_BY_VALUE_COPY _Pragma("clang diagnostic ignored \"-Wlarge-by-value-copy\"")
		#define WARNING_DISABLE_INCREASES_REQUIRED_ALIGNMENT _Pragma("clang diagnostic ignored \"-Wcast-align\"")
		#define WARNING_DISABLE_NONSTANDARD_EXTENSION_USED
	#else
		/* TODO: implement */
		#define WARNING_PUSH
		#define WARNING_POP 
		#define WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW
		#define WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED
		#define WARNING_DISABLE_LARGE_BY_VALUE_COPY
		#define WARNING_DISABLE_INCREASES_REQUIRED_ALIGNMENT
		#define WARNING_DISABLE_NONSTANDARD_EXTENSION_USED
	#endif

    #define _UNREACHABLE __builtin_unreachable()

    #define _ASSUME(condition) do { if (!(condition)) { __builtin_unreachable(); } } while(0)

	#ifndef STDLIB_DISABLE_DIAGNOSTIC_OVERRIDES
		#ifdef __clang__
			#pragma clang diagnostic error "-Wswitch"
			#pragma clang diagnostic error "-Wswitch-enum"
			#pragma clang diagnostic error "-Wundef"
			#pragma clang diagnostic warning "-Wshadow"
			#pragma clang diagnostic warning "-Wpointer-arith"
			#pragma clang diagnostic warning "-Wcast-align"
			#pragma clang diagnostic warning "-Wstrict-prototypes"
			#pragma clang diagnostic error "-Wwrite-strings"
			#pragma clang diagnostic warning "-Wold-style-cast"
			#pragma clang diagnostic error "-Wformat"
			#pragma clang diagnostic warning "-Wself-assign"
			#pragma clang diagnostic error "-Winstantiation-after-specialization"
			#pragma clang diagnostic warning "-Wlarge-by-value-copy"
			#pragma clang diagnostic warning "-Wpessimizing-move"
			//#pragma clang diagnostic warning "-Wextra-semi-stmt"
			//#pragma clang diagnostic warning "-Wempty-init-stmt"
		#else
			#pragma GCC diagnostic error "-Wswitch"
			#pragma GCC diagnostic error "-Wswitch-enum"
			#pragma GCC diagnostic error "-Wundef"
			#pragma GCC diagnostic warning "-Wshadow"
			#pragma GCC diagnostic warning "-Wpointer-arith"
			#pragma GCC diagnostic warning "-Wcast-align"
			#pragma GCC diagnostic warning "-Wstrict-prototypes"
			#pragma GCC diagnostic error "-Wwrite-strings"
			#pragma GCC diagnostic warning "-Wold-style-cast"
			#pragma GCC diagnostic error "-Wformat"
			#pragma GCC diagnostic warning "-Wself-assign"
			#pragma GCC diagnostic error "-Winstantiation-after-specialization"
			#pragma GCC diagnostic warning "-Wlarge-by-value-copy"
			#pragma GCC diagnostic warning "-Wpessimizing-move"
			//#pragma GCC diagnostic warning "-Wextra-semi-stmt"
			//#pragma GCC diagnostic warning "-Wempty-init-stmt"
#endif
	#endif

	using _IndexOfSignificantBitResultType = int;
	using _IndexOfSignificantBitInputType = unsigned long;
	using _IndexOfSignificantBitInput64Type = unsigned long long;

    #define _MSNZB32(tosearch, result) (*result = (31 - __builtin_clz(tosearch)))
    #define _LSNZB32(tosearch, result) (*result = __builtin_ctz(tosearch))

    #define _MSNZB64(tosearch, result) (*result = (63 - __builtin_clzll(tosearch)))
    #define _LSNZB64(tosearch, result) (*result = __builtin_ctzll(tosearch))

	using _ByteSwapInput16 = uint16_t;
	using _ByteSwapInput32 = uint32_t;
	using _ByteSwapInput64 = uint64_t;

    #define _BYTESWAP16(value) __builtin_bswap16(value)
    #define _BYTESWAP32(value) __builtin_bswap32(value)
    #define _BYTESWAP64(value) __builtin_bswap64(value)

    // TODO: doesn't compile
    //#define _ROTATE32R(val, shift) __builtin_rotateright32(val, shift)
    //#define _ROTATE32L(val, shift) __builtin_rotateleft32(val, shift)
    //#define _ROTATE64R(val, shift) __builtin_rotateright64(val, shift)
    //#define _ROTATE64L(val, shift) __builtin_rotateleft64(val, shift)
    //#define _ROTATE16R(val, shift) __builtin_rotateright16(val, shift)
    //#define _ROTATE16L(val, shift) __builtin_rotateleft16(val, shift)
    //#define _ROTATE8R(val, shift) __builtin_rotateright8(val, shift)
    //#define _ROTATE8L(val, shift) __builtin_rotateleft8(val, shift)

    // TODO: bit tests and sets

#else

    #error unknown compiler

#endif

inline bool __BitTestAndReset32(int *value, int position)
{
    bool isSet = (*value & (1 << position)) != 0;
    *value &= ~(1 << position);
    return isSet;
}

inline bool __BitTestAndSet32(int *value, int position)
{
    bool isSet = (*value & (1 << position)) != 0;
    *value |= 1 << position;
    return isSet;
}

inline bool __BitTestAndReset64(long long *value, long long position)
{
    bool isSet = (*value & (1LL << position)) != 0;
    *value &= ~(1LL << position);
    return isSet;
}

inline bool __BitTestAndSet64(long long *value, long long position)
{
    bool isSet = (*value & (1LL << position)) != 0;
    *value |= 1LL << position;
    return isSet;
}

#if !defined(_ROTATE64R) || !defined(_ROTATE64L)
	using _RotateBitsInput64 = unsigned long long;
    #define _ROTATE64R(val, shift) ((val >> shift) | (val << (64 - shift)))
    #define _ROTATE64L(val, shift) ((val << shift) | (val >> (64 - shift)))
#endif
#if !defined(_ROTATE32R) || !defined(_ROTATE32L)
	using _RotateBitsInput32 = unsigned int;
    #define _ROTATE32R(val, shift) ((val >> shift) | (val << (32 - shift)))
    #define _ROTATE32L(val, shift) ((val << shift) | (val >> (32 - shift)))
#endif
#if !defined(_ROTATE16R) || !defined(_ROTATE16L)
	using _RotateBitsInput16 = unsigned short;
    #define _ROTATE16R(val, shift) (((val >> shift) | (val << (16 - shift))) & 0xFFFF)
    #define _ROTATE16L(val, shift) (((val << shift) | (val >> (16 - shift))) & 0xFFFF)
#endif
#if !defined(_ROTATE8R) || !defined(_ROTATE8L)
	using _RotateBitsInput8 = unsigned char;
    #define _ROTATE8R(val, shift) (((val >> shift) | (val << (8 - shift))) & 0xFF)
    #define _ROTATE8L(val, shift) (((val << shift) | (val >> (8 - shift))) & 0xFF)
#endif

#if !defined(_MSNZB32) || !defined(_LSNZB32)
	using _IndexOfSignificantBitResultType = unsigned int;
	using _IndexOfSignificantBitInputType = unsigned int;

    #define _MSNZB32(tosearch, result) do { \
		ASSUME(tosearch != 0); \
		for (ui32 index = 31; ; --index) { \
			if (tosearch & 1u << index) { \
				*result = index; \
				break; } } } while(0)
    
	#define _LSNZB32(tosearch, result) do { \
		ASSUME(tosearch != 0); \
		for (ui32 index = 0; ; ++index) { \
			if (tosearch & 1u << index) { \
				*result = index; \
				break; } } } while(0)
#endif
#if !defined(_MSNZB64) || !defined(_LSNZB64)
	using _IndexOfSignificantBitInput64Type = unsigned long long; // assuming that _IndexOfSignificantBitResultType is already defined

    #define _MSNZB64(tosearch, result) do { \
		ASSUME(tosearch != 0); \
		unsigned highPart = static_cast<unsigned>(tosearch >> 32); \
		unsigned lowPart = static_cast<unsigned>(tosearch & 0xFFFFFFFF); \
		_IndexOfSignificantBitResultType tempResult; \
		if (highPart) { \
			_MSNZB32(highPart, &tempResult); \
			tempResult += 32; } \
		else \
			_MSNZB32(lowPart, &tempResult); \
		*result = tempResult; } while(0)
    
	#define _LSNZB64(tosearch, result) do { \
		ASSUME(tosearch != 0); \
		unsigned highPart = static_cast<unsigned>(tosearch >> 32); \
		unsigned lowPart = static_cast<unsigned>(tosearch & 0xFFFFFFFF); \
		_IndexOfSignificantBitResultType tempResult; \
		if (lowPart) \
			_LSNZB32(lowPart, &tempResult); \
		else { \
			_LSNZB32(highPart, &tempResult); \
			tempResult += 32; } \
		*result = tempResult; } while(0)
#endif

#if !defined(_BITTEST32) && !defined(_BITTESTANDRESET32) && !defined(_BITTESTANDSET32)
	using BitTestInputType = int;
#endif

#if !defined(_BITTEST64) && !defined(_BITTESTANDRESET64) && !defined(_BITTESTANDSET64)
	using BitTestInput64Type = long long;
#endif

#ifndef _BITTEST32
    #define _BITTEST32(value, position) ((value & (1 << position)) != 0)
#endif
#ifndef _BITTEST64
    #define _BITTEST64(value, position) ((value & (1LL << position)) != 0)
#endif
#ifndef _BITTESTANDRESET32
    #define _BITTESTANDRESET32(value, position) __BitTestAndReset32(value, position)
#endif
#ifndef _BITTESTANDSET32
    #define _BITTESTANDSET32(value, position) __BitTestAndSet32(value, position)
#endif
#ifndef _BITTESTANDRESET64
    #define _BITTESTANDRESET64(value, position) __BitTestAndReset64(value, position)
#endif
#ifndef _BITTESTANDSET64
    #define _BITTESTANDSET64(value, position) __BitTestAndSet64(value, position)
#endif