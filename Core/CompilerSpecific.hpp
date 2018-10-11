#pragma once

#ifdef _MSC_VER

    #pragma warning(1 : 4062) /* The enumerate has no associated handler in a switch statement, and there is no default label. */
    //#pragma warning(1 : 4265) /* class has virtual functions, but destructor is not virtual */
    #pragma warning(1 : 4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
    #pragma warning(1 : 4263) /* 'function': member function does not override any base class virtual member function */
    #pragma warning(1 : 4264) /* 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden */
    #pragma warning(1 : 4266) /* 'function': no override available for virtual member function from base 'type'; function is hidden */

    #define RSTR __restrict
    #define ALLOCA(count, sizeOfElement) _alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN __declspec(restrict)
    #define ALLOCATORFUNC __declspec(allocator)
    #define NOINLINE __declspec(noinline)
    #define FORCEINLINE __forceinline

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

#elif defined(__GNUC__)

    #define RSTR __restrict__
    #define ALLOCA(count, sizeOfElement) __builtin_alloca((count) * (sizeOfElement))
    #define UNIQUEPTRRETURN /* TODO: find out */
    #define ALLOCATORFUNC
    #define NOINLINE __attribute__((noinline))
    #define FORCEINLINE __attribute__((always_inline)) inline

    #define _UNREACHABLE __builtin_unreachable()

    #define _ASSUME(condition) do { if (!(condition)) { __builtin_unreachable(); } } while(0)

    // won't work with clang?
    #pragma GCC diagnostic error "-Wswitch"

    #define _MSNZB32(tosearch, result) do { ASSUME(*(unsigned int *)&tosearch != 0); *result = (31 - __builtin_clz(*(unsigned int *)&tosearch)); } while(0)
    #define _LSNZB32(tosearch, result) do { ASSUME(*(unsigned int *)&tosearch != 0); *result = __builtin_ctz(*(unsigned int *)&tosearch); } while(0)

    #define _MSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); *result = (63 - __builtin_clzll(*(unsigned long long *)&tosearch)); } while(0)
    #define _LSNZB64(tosearch, result) do { ASSUME(*(unsigned long long *)&tosearch != 0); *result = __builtin_ctzll(*(unsigned long long *)&tosearch); } while(0)

    #define _BYTESWAP16(value) __builtin_bswap16(value)
    #define _BYTESWAP32(value) __builtin_bswap32(value)
    #define _BYTESWAP64(value) __builtin_bswap64(value)

    // TODO: rotate intrinsics

#else

    #error unknown compiler

#endif

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