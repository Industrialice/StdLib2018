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

    #define RSTR __restrict
    #define ALLOCA(count, sizeOfElement) _alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN __declspec(restrict)
    #define ALLOCATORFUNC __declspec(allocator)
    #define NOINLINE __declspec(noinline)
    #define FORCEINLINE __forceinline
	#define UNALIGNEDPTR __unaligned
    #define EMPTY_BASES __declspec(empty_bases)
    #define NOVTABLE __declspec(novtable)

    #define _UNREACHABLE __assume(0)

    #define _ASSUME(condition) __assume(condition)

    #include <intrin.h>

    #define _MSNZB32(tosearch, result) do { ASSUME(*(unsigned long *)&tosearch != 0); unsigned long r; _BitScanReverse(&r, *(unsigned long *)&tosearch); *result = r; } while(0)
    #define _LSNZB32(tosearch, result) do { ASSUME(*(unsigned long *)&tosearch != 0); unsigned long r; _BitScanForward(&r, *(unsigned long *)&tosearch); *result = r; } while(0)

#ifdef _WIN64
    #define _MSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); unsigned long r; _BitScanReverse64(&r, *(unsigned long long *)&tosearch); *result = r; } while(0)
    #define _LSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); unsigned long r; _BitScanForward64(&r, *(unsigned long long *)&tosearch); *result = r; } while(0)
#endif

    #define _BYTESWAP16(value) _byteswap_ushort(value)
    #define _BYTESWAP32(value) _byteswap_ulong(value)
    #define _BYTESWAP64(value) _byteswap_uint64(value)

    #define _ROTATE32R(val, shift) _rotr(val, shift)
    #define _ROTATE32L(val, shift) _rotl(val, shift)
    #define _ROTATE64R(val, shift) _rotr64(val, shift)
    #define _ROTATE64L(val, shift) _rotl64(val, shift)
    #define _ROTATE16R(val, shift) _rotr16(val, shift)
    #define _ROTATE16L(val, shift) _rotl16(val, shift)
    #define _ROTATE8R(val, shift) _rotr8(val, shift)
    #define _ROTATE8L(val, shift) _rotl8(val, shift)

    // works, but shows worse performance
    /*#define _BITTEST32(value, position) _bittest((long *)value, (long)position)
    #if defined(_WIN64) || defined(_M_ARM)
        #define _BITTEST64(value, position) _bittest64((__int64 *)value, (__int64)position)
    #endif*/

    /*#define _BITTESTANDRESET32(value, position) _bittestandreset((long *)value, (long)position)
    #define _BITTESTANDSET32(value, position) _bittestandset((long *)value, (long)position)
    #if defined(_M_AMD64)
        #define _BITTESTANDRESET64(value, position) _bittestandreset64((__int64 *)value, (__int64)position)
        #define _BITTESTANDSET64(value, position) _bittestandset64((__int64 *)value, (__int64)position)
    #endif*/

#elif defined(__GNUC__) || defined(__clang__) || defined(__EMSCRIPTEN__) || defined(__MINGW32__) || defined(__MINGW32__) || defined(__MINGW64__)

    #define RSTR __restrict__
    #define ALLOCA(count, sizeOfElement) __builtin_alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN /* TODO: find out */
    #define ALLOCATORFUNC /* TODO: find out */
    #define NOINLINE __attribute__((noinline))
    #define FORCEINLINE __attribute__((always_inline)) inline
#ifdef __clang__
	#define UNALIGNEDPTR __unaligned /* It seems there's no alternative for GCC */
    #define NOVTABLE __declspec(novtable)
#else
    #define NOVTABLE /* doesn't seem to be supported by GCC */
#endif
    #define EMPTY_BASES /* TODO: do these compilers have something similar */

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
		#else
			#pragma GCC diagnostic error "-Wswitch"
			#pragma GCC diagnostic error "-Wswitch-enum"
			#pragma GCC diagnostic error "-Wundef"
			#pragma GCC diagnostic warning "-Wshadow"
			#pragma GCC diagnostic warning "-Wpointer-arith"
			#pragma GCC diagnostic warning "-Wcast-align"
			#pragma GCC diagnostic warning "-Wstrict-prototypes"
			#pragma GCC diagnostic error "-Wwrite-strings"
		#endif
	#endif

    #define _MSNZB32(tosearch, result) do { ASSUME(*(unsigned int *)&tosearch != 0); *result = (31 - __builtin_clz(*(unsigned int *)&tosearch)); } while(0)
    #define _LSNZB32(tosearch, result) do { ASSUME(*(unsigned int *)&tosearch != 0); *result = __builtin_ctz(*(unsigned int *)&tosearch); } while(0)

    #define _MSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); *result = (63 - __builtin_clzll(*(unsigned long long *)&tosearch)); } while(0)
    #define _LSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); *result = __builtin_ctzll(*(unsigned long long *)&tosearch); } while(0)

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

inline bool __BitTestAndReset32(long *value, long position)
{
    bool isSet = (*value & ((long)1 << position)) != 0;
    *value &= ~((long)1 << position);
    return isSet;
}

inline bool __BitTestAndSet32(long *value, long position)
{
    bool isSet = (*value & ((long)1 << position)) != 0;
    *value |= (long)1 << position;
    return isSet;
}

inline bool __BitTestAndReset64(long long *value, long long position)
{
    bool isSet = (*value & ((long long)1 << position)) != 0;
    *value &= ~((long long)1 << position);
    return isSet;
}

inline bool __BitTestAndSet64(long long *value, long long position)
{
    bool isSet = (*value & ((long long)1 << position)) != 0;
    *value |= (long long)1 << position;
    return isSet;
}

#ifndef _ROTATE64R
    #define _ROTATE64R(val, shift) ((ui64)((ui64)(val) >> (shift)) | (ui64)((ui64)(val) << (64 - (shift))))
#endif
#ifndef _ROTATE64L
    #define _ROTATE64L(val, shift) ((ui64)((ui64)(val) << (shift)) | (ui64)((ui64)(val) >> (64 - (shift))))
#endif
#ifndef _ROTATE32R
    #define _ROTATE32R(val, shift) ((ui32)((ui32)(val) >> (shift)) | (ui32)((ui32)(val) << (32 - (shift))))
#endif
#ifndef _ROTATE32L
    #define _ROTATE32L(val, shift) ((ui32)((ui32)(val) << (shift)) | (ui32)((ui32)(val) >> (32 - (shift))))
#endif
#ifndef _ROTATE16R
    #define _ROTATE16R(val, shift) ((ui16)((ui16)(val) >> (shift)) | (ui16)((ui16)(val) << (16 - (shift))))
#endif
#ifndef _ROTATE16L
    #define _ROTATE16L(val, shift) ((ui16)((ui16)(val) << (shift)) | (ui16)((ui16)(val) >> (16 - (shift))))
#endif
#ifndef _ROTATE8R
    #define _ROTATE8R(val, shift) ((ui8)((ui8)(val) >> (shift)) | (ui8)((ui8)(val) << (8 - (shift))))
#endif
#ifndef _ROTATE8L
    #define _ROTATE8L(val, shift) ((ui8)((ui8)(val) << (shift)) | (ui8)((ui8)(val) >> (8 - (shift))))
#endif

#ifndef _MSNZB32
    #define _MSNZB32(tosearch, result) do { ASSUME(tosearch != 0); for (ui32 index = 31; ; --index) { if (tosearch & 1 << index) { *result = index; break; } } } while(0)
#endif
#ifndef _LSNZB32
    #define _LSNZB32(tosearch, result) do { ASSUME(tosearch != 0); for (ui32 index = 0; ; ++index) { if (tosearch & 1 << index) { *result = index; break; } } } while(0)
#endif
#ifndef _MSNZB64
    #define _MSNZB64(tosearch, result) do { ASSUME(tosearch != 0); auto casted = *(unsigned long long *)&tosearch; unsigned highPart = unsigned(casted >> 32); unsigned lowPart = unsigned(casted & 0xFFFFFFFF); unsigned int tempResult; if (highPart) { _MSNZB32(highPart, &tempResult); tempResult += 32; } else _MSNZB32(lowPart, &tempResult); *result = tempResult; } while(0)
#endif
#ifndef _LSNZB64
    #define _LSNZB64(tosearch, result) do { ASSUME(tosearch != 0); auto casted = *(unsigned long long *)&tosearch; unsigned highPart = unsigned(casted >> 32); unsigned lowPart = (unsigned)(casted & 0xFFFFFFFF); unsigned int tempResult; if (lowPart) _LSNZB32(lowPart, &tempResult); else { _LSNZB32(highPart, &tempResult); tempResult += 32; } *result = tempResult; } while(0)
#endif
#ifndef _BITTEST32
    #define _BITTEST32(value, position) (*value & ((long)1 << position)) != 0
#endif
#ifndef _BITTEST64
    #define _BITTEST64(value, position) (*value & ((long long)1 << position)) != 0
#endif
#ifndef _BITTESTANDRESET32
    #define _BITTESTANDRESET32(value, position) __BitTestAndReset32((long *)value, (long)position)
#endif
#ifndef _BITTESTANDSET32
    #define _BITTESTANDSET32(value, position) __BitTestAndSet32((long *)value, (long)position)
#endif
#ifndef _BITTESTANDRESET64
    #define _BITTESTANDRESET64(value, position) __BitTestAndReset64((long long *)value, (long)position)
#endif
#ifndef _BITTESTANDSET64
    #define _BITTESTANDSET64(value, position) __BitTestAndSet64((long long *)value, (long)position)
#endif