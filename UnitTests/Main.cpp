#include <StdCoreLib.hpp>

using namespace StdLib;
using namespace Funcs;

static void SetBitTests()
{
    ui32 value = 0;
    SetBit(value, 14, true);
    ASSUME(value == 16384);
    ASSUME(IsBitSet(value, 14) == true);
    ASSUME(IsBitSet(value, 13) == false);
    SetBit(value, 14, false);
    ASSUME(value == 0);

    printf("finished set bit tests\n");
}

static void SignificantBitTests()
{
    ui64 ui64value = 16384;
    ASSUME(MostSignificantNonZeroBit(ui64value) == 14);
    ASSUME(LeastSignificantNonZeroBit(ui64value) == 14);
    ui64value = 18'446'744'073'709'551'615ULL;
    ASSUME(MostSignificantNonZeroBit(ui64value) == 63);
    ASSUME(LeastSignificantNonZeroBit(ui64value) == 63);

    i64 i64value = -9223372036854775808LL;
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

    printf("finished significant bit tests\n");
}

static void ChangeEndiannessTests()
{
    ui64 ui64value = 0xFF'00'00'FF'00'FF'00ULL;
    ASSUME(ChangeEndianness(ui64value) == 0x00'FF'00'FF'00'00'FFULL);
    ui32 ui32value = 0xFF'00'00'00;
    ASSUME(ChangeEndianness(ui32value) == 0x00'00'00'FF);
    ui16 ui16value = 0xFF'00;
    ASSUME(ChangeEndianness(ui16value) == 0x00'FF);
    ui8 ui8value = 0xFF;
    ASSUME(ChangeEndianness(ui8value) == 0xFF);

    printf("finished change endianness tests\n");
}

int main()
{
    StdLib::Initialization::CoreInitialize({});

    SetBitTests();
    SignificantBitTests();
    ChangeEndiannessTests();

    getchar();
}