#include <StdCoreLib.hpp>

#ifdef PLATFORM_ANDROID
    #include <android/log.h>
#endif

#ifdef PLATFORM_ANDROID
    #define PRINTLOG(...) __android_log_print(ANDROID_LOG_INFO, "StdLib", __VA_ARGS__)
#else
    #define PRINTLOG(...) printf(__VA_ARGS__)
#endif

using namespace StdLib;
using namespace Funcs;

static void SetBitTests()
{
    ui32 value = 0;
    value = SetBit(value, 14, true);
    ASSUME(value == 16384);
    ASSUME(IsBitSet(value, 14) == true);
    ASSUME(IsBitSet(value, 13) == false);
    value = SetBit(value, 14, false);
    ASSUME(value == 0);

    PRINTLOG("finished set bit tests\n");
}

static void SignificantBitTests()
{
    ui64 ui64value = 16384;
    ASSUME(MostSignificantNonZeroBit(ui64value) == 14);
    ASSUME(LeastSignificantNonZeroBit(ui64value) == 14);
    ui64value = 1ULL << 63;
    ASSUME(MostSignificantNonZeroBit(ui64value) == 63);
    ASSUME(LeastSignificantNonZeroBit(ui64value) == 63);

    i64 i64value = i64_min;
    ASSUME(MostSignificantNonZeroBit(i64value) == 63);
    ASSUME(LeastSignificantNonZeroBit(i64value) == 63);
    i64value = 15;
    ASSUME(MostSignificantNonZeroBit(i64value) == 3);
    ASSUME(LeastSignificantNonZeroBit(i64value) == 0);

    ui8 ui8value = 128;
    ASSUME(MostSignificantNonZeroBit(ui8value) == 7);
    ASSUME(LeastSignificantNonZeroBit(ui8value) == 7);
    ui8value = 15;
    ASSUME(MostSignificantNonZeroBit(ui8value) == 3);
    ASSUME(LeastSignificantNonZeroBit(ui8value) == 0);

    i8 i8value = -128;
    ASSUME(MostSignificantNonZeroBit(i8value) == 7);
    ASSUME(LeastSignificantNonZeroBit(i8value) == 7);
    i8value = 15;
    ASSUME(MostSignificantNonZeroBit(i8value) == 3);
    ASSUME(LeastSignificantNonZeroBit(i8value) == 0);

    PRINTLOG("finished significant bit tests\n");
}

static void ChangeEndiannessTests()
{
    ui64 ui64value = 0xFF'00'00'00'FF'00'00'00ULL;
    ASSUME(ChangeEndianness(ui64value) == 0x00'00'00'FF'00'00'00'FFULL);
    ui32 ui32value = 0xFF'00'00'00;
    ASSUME(ChangeEndianness(ui32value) == 0x00'00'00'FFU);
    ui16 ui16value = 0xFF'00;
    ASSUME(ChangeEndianness(ui16value) == 0x00'FF);
    ui8 ui8value = 0xFF;
    ASSUME(ChangeEndianness(ui8value) == 0xFF);

    PRINTLOG("finished change endianness tests\n");
}

static void RotateBitsTests()
{
    ui64 ui64value = 0b001;
    ui64value = RotateBitsRight(ui64value, 2);
    ASSUME(ui64value == 0x40'00'00'00'00'00'00'00ULL);
    ui64value = RotateBitsLeft(ui64value, 2);
    ASSUME(ui64value == 0b001);
    ui32 ui32value = 0b001;
    ui32value = RotateBitsRight(ui32value, 2);
    ASSUME(ui32value == 0x40'00'00'00U);
    ui32value = RotateBitsLeft(ui32value, 2);
    ASSUME(ui32value == 0b001);
    ui16 ui16value = 0b001;
    ui16value = RotateBitsRight(ui16value, 2);
    ASSUME(ui16value == 0x40'00);
    ui16value = RotateBitsLeft(ui16value, 2);
    ASSUME(ui16value == 0b001);
    ui8 ui8value = 0b001;
    ui8value = RotateBitsRight(ui8value, 2);
    ASSUME(ui8value == 0x40);
    ui8value = RotateBitsLeft(ui8value, 2);
    ASSUME(ui8value == 0b001);

    PRINTLOG("finished rotate bits tests\n");
}

int main()
{
    StdLib::Initialization::CoreInitialize({});

    SetBitTests();
    SignificantBitTests();
    ChangeEndiannessTests();
    RotateBitsTests();

#ifdef PLATFORM_WINDOWS
    getchar();
#endif
}