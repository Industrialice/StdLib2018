#include "_PreHeader.hpp"

using namespace StdLib;
using namespace Funcs;

void MathLibTests();
void UniqueIdManagerTests();
void UniqueIdManagerBenchmark();
void LoggerTests();
void MTTests();

static void MiscTests()
{
    ui32 *arr = static_cast<ui32 *>(ALLOCA(5, sizeof(ui32)));
    std::fill(arr, arr + 5, 1);
    for (uiw index = 0; index < 5; ++index)
    {
        UTest(Equal, arr[index], 1);
    }

    arr = ALLOCA_TYPED(5, ui32);
    std::fill(arr, arr + 5, 2);
    for (uiw index = 0; index < 5; ++index)
    {
        UTest(Equal, arr[index], 2);
    }

	struct ReinitializeTest
	{
		bool isMovedOut = false;
		i32 value;

		ReinitializeTest(i32 value) : value(value)
		{}

		ReinitializeTest(const ReinitializeTest &) = default;
		ReinitializeTest &operator = (const ReinitializeTest &) = default;

		ReinitializeTest(ReinitializeTest &&source) noexcept : value(source.value)
		{
			source.isMovedOut = true;
			isMovedOut = false;
		}

		ReinitializeTest &operator = (ReinitializeTest &&source) noexcept
		{
			value = source.value;
			source.isMovedOut = true;
			isMovedOut = false;
			return *this;
		}
	};

	ReinitializeTest t0(0);
	ReinitializeTest t1(1);

	Funcs::Reinitialize(t0, t1);

	UTest(Equal, t1.isMovedOut, false);
	UTest(Equal, t0.value, t1.value);

    MovableAtomic<ui32> atomicTest(15);
    UTest(Equal, atomicTest.load(), 15);
    MovableAtomic<ui32> atomicTest2 = std::move(atomicTest);
    UTest(Equal, atomicTest2.load(), 15);

	UTest(true, Funcs::IsAligned(reinterpret_cast<void *>(16), 4));
	UTest(false, Funcs::IsAligned(reinterpret_cast<void *>(13), 4));

	UTest(Equal, Funcs::AlignAs(1, 16), 16u);
	UTest(Equal, Funcs::AlignAs(3, 16), 16u);
	UTest(Equal, Funcs::AlignAs(16, 16), 16u);
	UTest(Equal, Funcs::AlignAs(17, 16), 32u);

    UnitTestsLogger::Message("finished misc tests\n");
}

namespace
{
	ui32 InvalidParametersReceived;
}

static void MemOpsTests()
{
	std::array<ui8, 16> arr0;
	std::array<ui8, 32> arr1;

	auto checkMem = [](const auto &memory, ui8 value)
	{
		for (const ui8 &test : memory)
		{
			UTest(Equal, test, value);
		}
	};

#ifdef PLATFORM_WINDOWS
	auto myInvalidParameterHandler = [](const wchar_t *expression, const wchar_t *function,	const wchar_t *file, unsigned int line,	uintptr_t pReserved)
	{
		++InvalidParametersReceived;
	};
	_set_invalid_parameter_handler(myInvalidParameterHandler);
#endif

	UTest(Equal, MemOps::Set(arr0.data(), 1, arr0.size()), arr0.data());
	checkMem(arr0, 1);

	UTest(Equal, MemOps::SetChecked(static_cast<ui8 *>(nullptr), 23, 1, 10), false);
	UTest(Equal, MemOps::SetChecked(arr0.data(), 5, 1, 10), false);
	UTest(Equal, MemOps::SetChecked(arr0.data(), arr0.size(), 2, arr0.size()), true);
	checkMem(arr0, 2);

	UTest(Equal, MemOps::Chr(arr0.data(), 2, arr0.size()), arr0.data());
	checkMem(arr0, 2);

	UTest(Equal, MemOps::Set(arr1.data(), 3, arr1.size()), arr1.data());
	UTest(Equal, MemOps::Chr(arr1.data(), 1, arr1.size()), nullptr);

	UTest(Equal, MemOps::Compare(arr0.data(), arr1.data(), arr0.size()), -1);
	UTest(Equal, MemOps::Compare(arr0.data(), arr0.data(), arr0.size()), 0);

	UTest(Equal, MemOps::Copy(arr0.data(), arr1.data(), arr0.size()), arr0.data());
	UTest(Equal, MemOps::Compare(arr0.data(), arr1.data(), arr0.size()), 0);
	checkMem(arr0, 3);

	UTest(Equal, MemOps::CopyChecked(static_cast<ui8 *>(nullptr), 42, arr0.data(), arr0.size()), false);
	UTest(Equal, MemOps::CopyChecked(arr0.data(), 5, arr0.data(), arr0.size()), false);
	UTest(Equal, MemOps::CopyChecked(arr0.data(), arr0.size(), arr1.data(), arr0.size()), true);
	UTest(Equal, MemOps::Compare(arr0.data(), arr1.data(), arr0.size()), 0);
	checkMem(arr0, 3);

	UTest(Equal, MemOps::SetChecked(static_cast<ui8 *>(nullptr), 42, 1, 20), false);
	UTest(Equal, MemOps::SetChecked(arr0.data(), 5, 1, 20), false);
	UTest(Equal, MemOps::SetChecked(arr0.data(), arr0.size(), 4, arr0.size()), true);
	checkMem(arr0, 4);

	UTest(Equal, MemOps::Move(arr0.data(), arr1.data(), arr0.size()), arr0.data());
	checkMem(arr0, 3);

	UTest(Equal, MemOps::Set(arr0.data(), 6, arr0.size()), arr0.data());
	checkMem(arr0, 6);

	UTest(Equal, MemOps::MoveChecked(static_cast<ui8 *>(nullptr), 20, arr0.data(), 10), false);
	UTest(Equal, MemOps::MoveChecked(arr0.data(), 5, arr0.data(), 10), false);
	UTest(Equal, MemOps::MoveChecked(arr0.data(), arr0.size(), arr1.data(), arr0.size()), true);
	checkMem(arr0, 3);

#ifdef PLATFORM_WINDOWS
	UTest(Equal, InvalidParametersReceived, 8);
#endif

	UnitTestsLogger::Message("finished memory operations tests\n");
}

static void TupleHelpersTests()
{
	std::tuple<ui32, f32, bool> tuple;

	using tuple0 = RemoveTupleElement<0, decltype(tuple)>;
	static_assert(std::is_same_v<std::tuple_element_t<0, tuple0>, f32>);
	static_assert(std::is_same_v<std::tuple_element_t<1, tuple0>, bool>);

	using tuple1 = RemoveTupleElement<1, decltype(tuple)>;
	static_assert(std::is_same_v<std::tuple_element_t<0, tuple1>, ui32>);
	static_assert(std::is_same_v<std::tuple_element_t<1, tuple1>, bool>);

	using tuple2 = RemoveTupleElement<2, decltype(tuple)>;
	static_assert(std::is_same_v<std::tuple_element_t<0, tuple2>, ui32>);
	static_assert(std::is_same_v<std::tuple_element_t<1, tuple2>, f32>);

	std::tuple<> emptyTuple;
	using emptyTuple2 = RemoveTupleElement<0, decltype(emptyTuple)>;
	static_assert(std::tuple_size_v<decltype(emptyTuple)> == std::tuple_size_v<emptyTuple2>);

	constexpr auto toArrayTuple = std::make_tuple(0, 1, 2, 3, 4);
	constexpr auto arrayFromTuple = TupleToArray(toArrayTuple);
	static_assert(arrayFromTuple.size() == 5);
	UTest(Equal, arrayFromTuple, std::make_array(0, 1, 2, 3, 4));

	constexpr auto toArrayEmptyTuple = std::tuple<>();
	constexpr auto emptyArrayFromTuple = TupleToArray(toArrayEmptyTuple);
	static_assert(emptyArrayFromTuple.empty());
	static_assert(std::is_same_v<decltype(emptyArrayFromTuple)::value_type, std::tuple<>>);

	constexpr auto emptyArrayFromTupleDefaulted = TupleToArray<decltype(toArrayEmptyTuple), TypeId>(toArrayEmptyTuple);
	static_assert(emptyArrayFromTuple.empty());
	static_assert(std::is_same_v<decltype(emptyArrayFromTupleDefaulted)::value_type, TypeId>);

	constexpr auto tupleFromArray = ArrayToTuple(arrayFromTuple);
	static_assert(std::tuple_size_v<decltype(tupleFromArray)>);
	static_assert(tupleFromArray == std::make_tuple(0, 1, 2, 3, 4));
}

static void CompileTimeSortingTests()
{
    static constexpr std::array<ui32, 5> source = {4, 0, 100, 1, 9};
    static constexpr auto result = SortCompileTime(source);
    UTest(Equal, result, std::array<ui32, 5>{0, 1, 4, 9, 100});

    struct Limited
    {
        ui32 value{};

        constexpr bool operator < (const Limited &other) const
        {
            return value < other.value;
        }
    };
    static constexpr std::array<Limited, 5> source2 = {15, 99, 12, 1, 0};
    static constexpr std::array<Limited, 5> result2 = SortCompileTime(source2);
    for (uiw index = 0; index < source2.size(); ++index)
    {
        static constexpr std::array<Limited, 5> ref = {0, 1, 12, 15, 99};
        UTest(Equal, result2[index].value, ref[index].value);
    }

    UnitTestsLogger::Message("finished compile time sorting tests\n");
}

static void CompileTimeStringsTests()
{
    auto compute = []() constexpr -> std::array<char, 28>
    {
        constexpr const char testString[] = "CompileTimeStringsTests";
        constexpr ui64 encoded[] = {
            CompileTimeStrings::EncodeASCII(testString, CountOf(testString), CompileTimeStrings::CharsPerNumber * 0),
            CompileTimeStrings::EncodeASCII(testString, CountOf(testString), CompileTimeStrings::CharsPerNumber * 1),
            CompileTimeStrings::EncodeASCII(testString, CountOf(testString), CompileTimeStrings::CharsPerNumber * 2)};

        std::array<char, 28> decoded{};
        CompileTimeStrings::DecodeASCII<encoded[0], encoded[1], encoded[2]>(decoded.data(), decoded.size());

        return decoded;
    };

    constexpr std::array<char, 28> result = compute(); // makes sure the func is consteval
    constexpr std::array<char, 28> ref = {"CompileTimeStringsTests"};
    UTest(Equal, result, ref);

    //printf("decoded string is %s\n", result.data());

    UnitTestsLogger::Message("finished compile time strings tests\n");
}

static void EnumCombinableTests()
{
    static constexpr struct Values : EnumCombinable<Values, ui32, true>
    {} Default = Values::Create(0),
		One = Values::Create(Funcs::BitPos(0)),
        Two = Values::Create(Funcs::BitPos(1));

	Values one = One;
	Values two = Two;

    UTest(Equal, One.AsInteger(), 1);
    UTest(Equal, Two.AsInteger(), 2);
    UTest(Equal, One, One);
    UTest(NotEqual, One, Two);
    UTest(LeftLesser, One, Two);
    UTest(LeftGreater, Two, One);
	UTest(Equal, One, one);
	UTest(Equal, one, one);
	UTest(Equal, one, One);
	UTest(NotEqual, One, two);
	UTest(NotEqual, one, Two);
	UTest(NotEqual, one, two);
	UTest(LeftLesser, One, two);
	UTest(LeftLesser, one, Two);
	UTest(LeftLesser, one, two);

    Values value = One;
    value.Add(Two);

    UTest(true, value.Contains(Two));
    UTest(true, value.Contains(One));

    value.Remove(One);

    UTest(false, value.Contains(One));
    UTest(true, value.Contains(Two));

    UTest(true, value.Combined(One).Contains(One));

    UnitTestsLogger::Message("finished enum combinable tests\n");
}

WARNING_PUSH
WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW
static void TypeIdentifiableTests()
{
	struct S1 {};
	struct S2 {};

	constexpr TypeId s1Id = TypeIdentifiable<S1>::GetTypeId();
	constexpr TypeId s2Id = TypeIdentifiable<S2>::GetTypeId();
	UTest(NotEqual, s1Id, s2Id);
	ui64 s1Hash = s1Id.Hash();
	ui64 s2Hash = s2Id.Hash();
	UTest(NotEqual, s1Hash, s2Hash);

	constexpr TypeId ui32Id = TypeIdentifiable<ui32>::GetTypeId();
	constexpr TypeId ui8Id = TypeIdentifiable<ui8>::GetTypeId();
	UTest(NotEqual, ui32Id, ui8Id);
	ui64 ui32Hash = ui32Id.Hash();
	ui64 ui8Hash = ui8Id.Hash();
	UTest(NotEqual, ui32Hash, ui8Hash);
	UTest(Equal, TypeIdentifiable<i32>::GetTypeName(), "int");
	UTest(Equal, TypeIdentifiable<ui8>::GetTypeName(), "unsigned char");

	using TypeId3 = TypeIdentifiable<int>;
	using TypeId4 = TypeIdentifiable<float>;
	constexpr TypeId id3 = TypeId3::GetTypeId();
	constexpr TypeId id4 = TypeId4::GetTypeId();
	UTest(NotEqual, id3, id4);
	constexpr ui64 id3Hash = id3.Hash();
	constexpr ui64 id4Hash = id4.Hash();
	UTest(NotEqual, id3Hash, id4Hash);
    UTest(Equal, TypeId3::GetTypeName(), "int");
    UTest(Equal, TypeId4::GetTypeName(), "float");

#ifdef DEBUG
    constexpr TypeId idDebug = TypeIdentifiable<int>::GetTypeId();
    UTest(Equal, std::string(idDebug.Name()), "int"s);
#endif

	UnitTestsLogger::Message("finished type identifiable tests\n");
}

template <Hash::Precision precision, typename T> static void TestIntegerHashes()
{
    T number0 = 3;
    T number1 = 2;
    T number2 = 5;

    UTest(NotEqual, Hash::FNVHash<precision>(number0) + Hash::FNVHash<precision>(number1), Hash::FNVHash<precision>(number2));

    auto hash0 = Hash::Integer<precision>(number0);
	auto hash1 = Hash::Integer<precision>(number1);
	auto hash2 = Hash::Integer<precision>(number2);

    UTest(NotEqual, hash0, hash1);
    UTest(NotEqual, hash1, hash2);

    UTest(NotEqual, hash0 + hash1, hash2);

    UTest(Equal, Hash::IntegerInverse<precision>(hash0), 3);
    UTest(Equal, Hash::IntegerInverse<precision>(hash1), 2);
    UTest(Equal, Hash::IntegerInverse<precision>(hash2), 5);
}

static void HashFuncsTest()
{
	constexpr ui32 value = 23534;

	constexpr ui32 fnv32 = Hash::FNVHash<Hash::Precision::P32>(value);
	UTest(NotEqual, fnv32, value);

	constexpr ui64 fnv64 = Hash::FNVHash<Hash::Precision::P64>(value);
	UTest(NotEqual, fnv64, value);

	UTest(NotEqual, fnv32, fnv64);

	constexpr ui32 valuect[1] = {23534};
	constexpr ui32 fnv32ct = Hash::FNVHashCT<Hash::Precision::P32>(valuect);
	UTest(Equal, fnv32ct, Hash::FNVHash<Hash::Precision::P32>(23534));

	constexpr ui8 name[] = "Test Name";
	constexpr ui64 nameHashedCT = Hash::FNVHashCT<Hash::Precision::P64>(name);
	ui64 nameHashed = Hash::FNVHash<Hash::Precision::P64>(name, CountOf(name));
	UTest(Equal, nameHashedCT, nameHashed);

	const char *crc32str = "Skellig Peaceful Theme.mp3";
	ui32 crc32 = Hash::CRC32(reinterpret_cast<const ui8 *>(crc32str));
	UTest(Equal, crc32, 0xD85554CE);
	crc32 = Hash::CRC32(reinterpret_cast<const ui8 *>(crc32str), strlen(crc32str));
	UTest(Equal, crc32, 0xD85554CE);

    TestIntegerHashes<Hash::Precision::P64, ui8>();
	TestIntegerHashes<Hash::Precision::P32, ui8>();
	TestIntegerHashes<Hash::Precision::P32, ui16>();
	TestIntegerHashes<Hash::Precision::P64, ui16>();
	TestIntegerHashes<Hash::Precision::P32, ui32>();
	TestIntegerHashes<Hash::Precision::P64, ui32>();
	TestIntegerHashes<Hash::Precision::P32, ui64>();
	TestIntegerHashes<Hash::Precision::P64, ui64>();
	TestIntegerHashes<Hash::Precision::P32, i8>();
	TestIntegerHashes<Hash::Precision::P64, i8>();
	TestIntegerHashes<Hash::Precision::P32, i16>();
	TestIntegerHashes<Hash::Precision::P64, i16>();
	TestIntegerHashes<Hash::Precision::P32, i32>();
	TestIntegerHashes<Hash::Precision::P64, i32>();
	TestIntegerHashes<Hash::Precision::P32, i64>();
	TestIntegerHashes<Hash::Precision::P64, i64>();

	UnitTestsLogger::Message("finished hash tests\n");
}
WARNING_POP

static void IntegerPropertiesTest()
{
	UTest(false, IsPowerOf2(0));
	UTest(true, IsPowerOf2(16u));
	UTest(false, IsPowerOf2(15u));
	UTest(true, IsPowerOf2(16));
	UTest(false, IsPowerOf2(15));
	UTest(false, IsPowerOf2(-5));

	UnitTestsLogger::Message("finished integer properties tests\n");
}

static void SetBitTests()
{
	ui32 value = 0;
	value = SetBit(value, 14u, true);
	UTest(Equal, value, 16384u);
	UTest(true, IsBitSet(value, 14u));
	UTest(false, IsBitSet(value, 13u));
	value = SetBit(value, 14u, false);
	UTest(Equal, value, 0u);
	ui64 value64 = SetBit<ui64>(0, 63, 1);
	UTest(Equal, value64, 0x8000'0000'0000'0000ULL);

	auto test = [](auto typedefining)
	{
		using type = decltype(typedefining);
		constexpr ui32 bitsCount = sizeof(type) * 8;
		std::unordered_set<ui32> setBits;

		auto testSet = [&setBits](type value)
		{
			for (ui32 index = 0; index < bitsCount; ++index)
			{
				if (setBits.find(index) != setBits.end())
				{
					UTest(true, IsBitSet(value, index));
				}
				else
				{
					UTest(false, IsBitSet(value, index));
				}
			}
		};

		type value = {};
		testSet(value);

		auto modify = [&value, &setBits, &testSet](ui32 bitNum, bool isSet)
		{
			value = SetBit(value, bitNum, isSet);
			if (isSet)
			{
				setBits.insert(bitNum);
			}
			else
			{
				setBits.erase(bitNum);
			}
			testSet(value);
		};

		modify(7u, true);
		modify(0u, true);
		modify(bitsCount - 1, true);
		modify(7u, false);
		modify(bitsCount - 2, true);
		modify(1u, true);
		modify(bitsCount - 1, false);
		modify(bitsCount - 1, false);
		if constexpr (sizeof(type) >= 2)
		{
			modify(14u, true);
			modify(14u, false);
			if constexpr (sizeof(type) >= 4)
			{
				modify(30u, true);
				modify(30u, false);
			}
			if constexpr (sizeof(type) == 8)
			{
				modify(60u, true);
				modify(60u, false);
			}
		}
	};

	struct Opaque
	{
		f32 fp = 0;
		i32 i = 0;
	};

	test(ui8());
	test(ui16());
	test(ui32());
	test(ui64());
	test(i8());
	test(i16());
	test(i32());
	test(i64());
	test(f32());
	test(f64());
	test(Opaque());

    UnitTestsLogger::Message("finished set bit tests\n");
}

static void SignificantBitTests()
{
    ui64 ui64value = 16384;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(ui64value), 14u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(ui64value), 14u);
    ui64value = 1ULL << 63;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(ui64value), 63u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(ui64value), 63u);

    i64 i64value = i64_min;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(i64value), 63u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(i64value), 63u);
    i64value = 15;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(i64value), 3u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(i64value), 0u);

    ui8 ui8value = 128;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(ui8value), 7u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(ui8value), 7u);
    ui8value = 15;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(ui8value), 3u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(ui8value), 0u);

    i8 i8value = -128;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(i8value), 7u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(i8value), 7u);
    i8value = 15;
    UTest(Equal, IndexOfMostSignificantNonZeroBit(i8value), 3u);
    UTest(Equal, IndexOfLeastSignificantNonZeroBit(i8value), 0u);

    ui64value = 16384;
    UTest(Equal, IndexOfMostSignificantZeroBit(ui64value), 63u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(ui64value), 0u);
    ui64value += 1;
    UTest(Equal, IndexOfMostSignificantZeroBit(ui64value), 63u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(ui64value), 1u);

    i64value = i64_min + 1;
    UTest(Equal, IndexOfMostSignificantZeroBit(i64value), 62u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(i64value), 1u);

    ui8value = 128;
    UTest(Equal, IndexOfMostSignificantZeroBit(ui8value), 6u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(ui8value), 0u);
    ui8value += 1;
    UTest(Equal, IndexOfMostSignificantZeroBit(ui8value), 6u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(ui8value), 1u);

    i8value = -127;
    UTest(Equal, IndexOfMostSignificantZeroBit(i8value), 6u);
    UTest(Equal, IndexOfLeastSignificantZeroBit(i8value), 1u);

    UnitTestsLogger::Message("finished significant bit tests\n");
}

static void ChangeEndiannessTests()
{
    ui64 ui64value = 0xFF'00'00'00'FF'00'00'00ULL;
    UTest(Equal, ChangeEndianness(ui64value), 0x00'00'00'FF'00'00'00'FFULL);
    ui32 ui32value = 0xFF'00'00'00;
    UTest(Equal, ChangeEndianness(ui32value), 0x00'00'00'FFU);
    ui16 ui16value = 0xFF'00;
    UTest(Equal, ChangeEndianness(ui16value), 0x00'FF);
    ui8 ui8value = 0xFF;
    UTest(Equal, ChangeEndianness(ui8value), 0xFF);

    UnitTestsLogger::Message("finished change endianness tests\n");
}

static void RotateBitsTests()
{
    ui64 ui64value = 0b001;
    ui64value = RotateBitsRight(ui64value, 2);
    UTest(Equal, ui64value, 0x40'00'00'00'00'00'00'00ULL);
    ui64value = RotateBitsLeft(ui64value, 2);
    UTest(Equal, ui64value, 0b001);
    ui32 ui32value = 0b001;
    ui32value = RotateBitsRight(ui32value, 2);
    UTest(Equal, ui32value, 0x40'00'00'00U);
    ui32value = RotateBitsLeft(ui32value, 2);
    UTest(Equal, ui32value, 0b001);
    ui16 ui16value = 0b001;
    ui16value = RotateBitsRight(ui16value, 2);
    UTest(Equal, ui16value, 0x40'00);
    ui16value = RotateBitsLeft(ui16value, 2);
    UTest(Equal, ui16value, 0b001);
    ui8 ui8value = 0b001;
    ui8value = RotateBitsRight(ui8value, 2);
    UTest(Equal, ui8value, 0x40);
    ui8value = RotateBitsLeft(ui8value, 2);
    UTest(Equal, ui8value, 0b001);

    UnitTestsLogger::Message("finished rotate bits tests\n");
}

static void ErrorTests()
{
    constexpr auto okError = DefaultError::Ok();
    UTest(true, okError.IsOk());
    UTest(false, okError);
    UTest(Equal, okError, DefaultError::Ok());
    UTest(NotEqual, okError, DefaultError::UnknownError());

    auto unknownFormatWithAttachment = Error<std::string>(DefaultError::UnknownFormat(), "MPEG");
    UTest(false, unknownFormatWithAttachment.IsOk());
    UTest(true, unknownFormatWithAttachment);
    UTest(Equal, unknownFormatWithAttachment, DefaultError::UnknownFormat());
    UTest(Equal, unknownFormatWithAttachment.Attachment(), "MPEG");

    auto busyWithCustomDescription = Error<void, std::string>(DefaultError::Busy(), "Bus was handling "s + std::to_string(284) + " other requests"s);
    UTest(false, busyWithCustomDescription.IsOk());
    UTest(true, busyWithCustomDescription);
    UTest(Equal, busyWithCustomDescription, DefaultError::Busy());
    UTest(Equal, busyWithCustomDescription.Description(), "Bus was handling "s + std::to_string(284) + " other requests"s);

    struct Canceller {};
    auto canceller = std::make_shared<Canceller>();
    auto cancelledWithAttachmentAndCustomDescription = Error<std::shared_ptr<Canceller>, std::string>(DefaultError::Cancelled(), canceller, "Cancelling reason: "s + "interrupted by user"s);
    UTest(false, cancelledWithAttachmentAndCustomDescription.IsOk());
    UTest(true, cancelledWithAttachmentAndCustomDescription);
    UTest(Equal, cancelledWithAttachmentAndCustomDescription, DefaultError::Cancelled());
    UTest(true, AreSharedPointersEqual(cancelledWithAttachmentAndCustomDescription.Attachment(), canceller));
    UTest(Equal, cancelledWithAttachmentAndCustomDescription.Description(), "Cancelling reason: "s + "interrupted by user"s);

    cancelledWithAttachmentAndCustomDescription = decltype(cancelledWithAttachmentAndCustomDescription)::FromDefaultError(DefaultError::Ok());
    UTest(true, cancelledWithAttachmentAndCustomDescription.IsOk());
    UTest(false, cancelledWithAttachmentAndCustomDescription);
    UTest(Equal, cancelledWithAttachmentAndCustomDescription, DefaultError::Ok());
    UTest(Equal, cancelledWithAttachmentAndCustomDescription.Attachment(), std::shared_ptr<Canceller>());
    UTest(Equal, cancelledWithAttachmentAndCustomDescription.Description(), "");

    struct CustomErrorTest : public _ErrorCodes::ErrorCodeBase
    {
        CustomErrorTest() : ErrorCodeBase(0, "CustomErrorTest")
        {}
    };
    auto createCustomErrorUnitTest = []
    {
        return Error<>(CustomErrorTest(), "UnitTest", nullptr);
    };
    auto customErrorWithAttachmentAndCustomDescription = Error<i32, std::string>(createCustomErrorUnitTest(), -45, "UnitTesting "s + std::to_string(-45));
    UTest(false, customErrorWithAttachmentAndCustomDescription.IsOk());
    UTest(true, customErrorWithAttachmentAndCustomDescription);
    UTest(Equal, customErrorWithAttachmentAndCustomDescription, createCustomErrorUnitTest());
    UTest(Equal, customErrorWithAttachmentAndCustomDescription.Attachment(), -45);
    UTest(Equal, customErrorWithAttachmentAndCustomDescription.Description(), "UnitTesting "s + std::to_string(-45));

    UnitTestsLogger::Message("finished error tests\n");
}

static void ResultTests()
{
    Result<i32> intResult(43);
    UTest(true, intResult.IsOk());
    UTest(true, intResult);
    UTest(Equal, intResult.GetError(), DefaultError::Ok());
    UTest(Equal, intResult.Unwrap(), 43);

	Result<std::string> stringResult("testString");
	UTest(Equal, stringResult.Unwrap(), "testString");
	stringResult = std::string("testString");
	Result<std::string> stringResult2 = std::move(stringResult);
	UTest(Equal, stringResult2.Unwrap(), "testString");
	stringResult2 = std::string("testString");
	stringResult = std::move(stringResult2);
	UTest(Equal, stringResult.Unwrap(), "testString");
	stringResult = DefaultError::AccessDenied();
	UTest(false, stringResult.IsOk());
	UTest(Equal, stringResult.UnwrapOrGet("failed"), std::string("failed"));

    UnitTestsLogger::Message("finished result tests\n");
}

#ifdef PLATFORM_WINDOWS
    #define EXCEPTION_CHECK(Code, IsExpectingException) \
        { \
            bool isExcepted = false; \
            __try \
            { \
                Code; \
            } \
            __except (EXCEPTION_EXECUTE_HANDLER) \
            { \
                isExcepted = true; \
            } \
            UTest(Equal, isExcepted, IsExpectingException); \
        }
#else
    #define EXCEPTION_CHECK(Code, IsExpectingException)
#endif

static void VirtualMemoryTests()
{
#ifdef PLATFORM_WINDOWS
	if (SystemInfo::IsDebuggerAttached()) // debugger will throw an exception even for intercepted exceptions
	{
		UnitTestsLogger::Message("debugger is present, skipping virtual memory tests\n");
		return;
	}
#endif

    std::byte *memory = static_cast<std::byte *>(VirtualMemory::Reserve(999));
    UTest(true, memory);
    EXCEPTION_CHECK(MemOps::Set(memory, 0, 10), true);

    auto commitError = VirtualMemory::Commit(memory, 500, VirtualMemory::PageModes::Read.Combined(VirtualMemory::PageModes::Write));
    UTest(true, commitError.IsOk());
    EXCEPTION_CHECK(MemOps::Set(memory, 0, 10), false);

    auto protection = VirtualMemory::PageMode(memory, VirtualMemory::PageSize());
#ifdef PLATFORM_WINDOWS
    UTest(Equal, protection.Unwrap(), VirtualMemory::PageModes::Read.Combined(VirtualMemory::PageModes::Write));
#else
    UTest(Equal, protection.GetError(), DefaultError::Unsupported());
#endif

    auto protectionSetResult = VirtualMemory::PageMode(memory, VirtualMemory::PageSize(), VirtualMemory::PageModes::Read);
    UTest(true, protectionSetResult.IsOk());
    EXCEPTION_CHECK(MemOps::Set(memory, 0, 10), true);

    UTest(true, VirtualMemory::Free(memory, 999));

    memory = static_cast<std::byte *>(VirtualMemory::Alloc(999, VirtualMemory::PageModes::Read.Combined(VirtualMemory::PageModes::Write)));
    UTest(true, memory);
    EXCEPTION_CHECK(MemOps::Set(memory, 0, 10), false);

    UTest(true, VirtualMemory::Free(memory, 999));

    UnitTestsLogger::Message("finished virtual memory tests\n");
}

static void AllocatorsTests()
{
    uiw blockSize = 0;

	ui8 *memory = Allocator::Malloc::Allocate<ui8>(100);
    UTest(true, memory);

    blockSize = Allocator::Malloc::MemorySize(memory);
    UTest(LeftGreaterEqual, blockSize, 100u);

    bool expandResult = Allocator::Malloc::ReallocateInplace(memory, 200);
    if (expandResult)
    {
        blockSize = Allocator::Malloc::MemorySize(memory);
        UTest(LeftGreaterEqual, blockSize, 200u);
    }
    memory = Allocator::Malloc::Reallocate(memory, 300u);
    UTest(true, memory);

    blockSize = Allocator::Malloc::MemorySize(memory);
    UTest(LeftGreaterEqual, blockSize, 300u);

    Allocator::Malloc::Free(memory);

    memory = Allocator::Malloc::Allocate<ui8>(0);
    memory = Allocator::Malloc::Reallocate(memory, 0);
#ifdef PLATFORM_WINDOWS
    UTest(true, Allocator::Malloc::ReallocateInplace(memory, 0));
#endif
    Allocator::Malloc::Free(memory);

	memory = Allocator::MallocAligned::Allocate<4, ui8>(111);
	MemOps::Set(memory, 0x66, 111);
	UTest(true, Funcs::IsAligned(memory, 4));
	memory = Allocator::MallocAligned::Reallocate<4>(memory, 222);
	for (uiw index = 0; index < 111; ++index)
	{
		UTest(Equal, memory[index], 0x66);
	}
	UTest(true, Funcs::IsAligned(memory, 4));
	blockSize = Allocator::MallocAligned::MemorySize<4>(memory);
	UTest(LeftGreaterEqual, blockSize, 222u);
	Allocator::MallocAligned::Free<4>(memory);

	memory = Allocator::MallocAligned::Allocate<64, ui8>(31);
	UTest(true, Funcs::IsAligned(memory, 64));
	MemOps::Set(memory, 0x77, 31);
	memory = Allocator::MallocAligned::Reallocate<64>(memory, 11);
	UTest(true, Funcs::IsAligned(memory, 64));
	for (uiw index = 0; index < 11; ++index)
	{
		UTest(Equal, memory[index], 0x77);
	}
	blockSize = Allocator::MallocAligned::MemorySize<64>(memory);
	UTest(LeftGreaterEqual, blockSize, 11u);
	Allocator::MallocAligned::Free<64>(memory);

	memory = Allocator::MallocAligned::Allocate<1, ui8>(10);
	blockSize = Allocator::MallocAligned::MemorySize<1>(memory);
	UTest(LeftGreaterEqual, blockSize, 10u);
	Allocator::MallocAligned::Free<1>(memory);

	// testing MemorySize specifically, on Android targets prior 17 there's an unreliable hack solution to get it
	memory = Allocator::Malloc::Allocate<ui8>(0);
    for (uiw index = 0; index < 1000; ++index)
    {
        uiw size = rand() % 256;
		memory = Allocator::Malloc::Reallocate(memory, size);
        uiw newSize = Allocator::Malloc::MemorySize(memory);
        ASSUME(newSize >= size);
    }
    Allocator::Malloc::Free(memory);

    UnitTestsLogger::Message("finished allocators tests\n");
}

static void FilePathTests()
{
    FilePath path{TSTR("C:\\ext\\\\no.txt")};

    path.Normalize();
    UTest(Equal, path.PlatformPath(), TSTR("C:/ext/no.txt"));
    auto fileName = path.FileNameView();
    UTest(Equal, fileName.value(), TSTR("no"));
    auto fileExt = path.ExtensionView();
    UTest(Equal, fileExt.value(), TSTR("txt"));
    path.ReplaceExtension(TSTR("bmp"));
    fileExt = path.ExtensionView();
    UTest(Equal, fileExt.value(), TSTR("bmp"));
    path.ReplaceFileName(TSTR("yes"));
    fileName = path.FileNameView();
    UTest(Equal, fileName.value(), TSTR("yes"));
    UTest(Equal, path.PlatformPath(), TSTR("C:/ext/yes.bmp"));
    UTest(Equal, path.FileNameExtView().value(), TSTR("yes.bmp"));
    path.RemoveLevel();
    UTest(Equal, path.PlatformPath(), TSTR("C:/ext/"));
    path.RemoveLevel();
    UTest(Equal, path.PlatformPath(), TSTR("C:/"));
    path += TSTR("folder");
    UTest(Equal, path.PlatformPath(), TSTR("C:/folder"));
    UTest(Equal, path.FileNameView().value(), TSTR("folder"));
    UTest(Equal, path.FileNameExtView().value(), TSTR("folder"));
    UTest(true, path.HasFileName());
    UTest(false, path.ExtensionView());
    UTest(false, path.HasExtension());
    path /= TSTR("file.ext");
    UTest(Equal, path.PlatformPath(), TSTR("C:/folder/file.ext"));
    path.ReplaceFileNameExt(TSTR("new.fil"));
    UTest(Equal, path.PlatformPath(), TSTR("C:/folder/new.fil"));

    UnitTestsLogger::Message("finished file path tests\n");
}

template <typename T> static void FileTypeTest(IFile &file)
{
	UTest(Equal, T().Type(), file.Type());
}

static void FileWrite(IFile &file)
{
    UTest(Equal, file.ProcMode().Combined(FileProcModes::Write), file.ProcMode());
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), 0);
    
    std::string_view str = "test0123456789 start";
    ui32 written = 0;
    UTest(true, file.Write(str.data(), static_cast<ui32>(str.length()), &written));
    UTest(Equal, written, str.length());
    UTest(true, file.Write(nullptr, 0, &written));
    UTest(Equal, written, 0);
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), str.length());
    UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0);
    UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), 0);
    
    if (file.IsSeekSupported())
    {
        i64 oldOffset = file.Offset().Unwrap();
        UTest(true, file.Offset(FileOffsetMode::FromCurrent, -5));
        UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), str.length() - 5);
        UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0);
        UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), -5);
        str = "trats";
        UTest(true, file.Write(str.data(), static_cast<ui32>(str.length()), &written));
        UTest(Equal, written, str.length());
        UTest(Equal, file.Offset().Unwrap(), oldOffset);
    }

    uiw startSize = "test0123456789 start"s.length();
    UTest(Equal, file.Size().Unwrap(), startSize);
    UTest(false, file.Size(startSize - 1));
    UTest(Equal, file.Size().Unwrap(), startSize - 1);
    UTest(false, file.Size(startSize + 1));
    UTest(Equal, file.Size().Unwrap(), startSize + 1);
    UTest(false, file.Size(startSize - 1));
    UTest(Equal, file.Size().Unwrap(), startSize - 1);
}

static void FileRead(IFile &file)
{
    UTest(Equal, file.ProcMode(), FileProcModes::Read);
    UTest(Equal, file.Offset().Unwrap(), 0);

    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat" : "test0123456789 star";
    UTest(LeftGreaterEqual, file.Size().Unwrap(), str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    UTest(true, file.Read(target.data(), static_cast<ui32>(target.length()), &read));
    UTest(Equal, read, str.length());
    UTest(Equal, str, target);
    UTest(Equal, file.Offset().Unwrap(), str.length());
    UTest(true, file.Read(nullptr, 0, &read));
    UTest(Equal, read, 0);
    UTest(Equal, file.Offset().Unwrap(), str.length());

    if (file.IsSeekSupported())
    {
        UTest(true, file.Offset(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        UTest(true, file.Read(target.data(), static_cast<ui32>(target.length()), &read));
        UTest(Equal, read, str.length());
        UTest(Equal, target, str);
    }
}

static void FileReadOffsetted(IFile &file, uiw offset)
{
    UTest(Equal, file.ProcMode(), FileProcModes::Read);
    UTest(Equal, file.Offset().Unwrap(), 0);

    std::string_view str = (file.IsSeekSupported() ? "test0123456789 trat" : "test0123456789 star") + offset;
    UTest(LeftGreaterEqual, file.Size().Unwrap(), str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    UTest(true, file.Read(target.data(), static_cast<ui32>(target.length()), &read));
    UTest(Equal, read, str.length());
    UTest(Equal, str, target);
    UTest(Equal, file.Offset().Unwrap(), str.length());
    UTest(true, file.Read(nullptr, 0, &read));
    UTest(Equal, read, 0);
    UTest(Equal, file.Offset().Unwrap(), str.length());

    if (file.IsSeekSupported())
    {
        UTest(true, file.Offset(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        UTest(true, file.Read(target.data(), static_cast<ui32>(target.length()), &read));
        UTest(Equal, read, str.length());
        UTest(Equal, target, str);
    }
}

static void FileAppendWrite(IFile &file)
{
    UTest(Equal, file.ProcMode().Combined(FileProcModes::Write), file.ProcMode());
    UTest(Equal, file.Offset().Unwrap(), 0);
    std::string_view str = "9184";
    ui32 written = 0;
    UTest(true, file.Write(str.data(), static_cast<ui32>(str.length()), &written));
    UTest(Equal, written, str.length());
    UTest(Equal, file.Offset().Unwrap(), str.length());
    UTest(Equal, file.Size().Unwrap(), str.length());
}

static void FileAppendRead(IFile &file)
{
    UTest(Equal, file.ProcMode(), FileProcModes::Read);
    UTest(Equal, file.Offset().Unwrap(), 0);
    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat9184" : "test0123456789 star9184";
    UTest(Equal, file.Size().Unwrap(), str.length());
    std::string target(str.length(), '\0');
    ui32 read = 0;
    UTest(true, file.Read(target.data(), static_cast<ui32>(target.length()), &read));
    UTest(Equal, read, str.length());
    UTest(Equal, target, str);
}

static void FileWriteRead(IFile &file)
{
    std::string crapString0(256, '\0');
    std::string crapString1(128, '\0');

    for (char &ch : crapString0)
    {
        ch = 32 + rand() % 96;
    }
    for (char &ch : crapString1)
    {
        ch = 32 + rand() % 96;
    }

    ui32 written = 0xBAADDEAD, read = 0xFACCF00D;

    char readBuf[256];

    UTest(Equal, file.ProcMode().Combined(FileProcModes::Write).Combined(FileProcModes::Read), file.ProcMode());

    UTest(Equal, file.Offset().Unwrap(), 0);
    UTest(true, file.Write(crapString0.data(), static_cast<ui32>(crapString0.length()), &written));
    UTest(Equal, written, static_cast<ui32>(crapString0.length()));
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), static_cast<i64>(crapString0.length()));
    UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), 0LL);
    UTest(Equal, file.Size().Unwrap(), crapString0.length());

    UTest(true, file.Offset(FileOffsetMode::FromBegin, 32LL));
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), 32LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), -(static_cast<i64>(crapString0.length()) - 32));
    UTest(true, file.Read(readBuf, static_cast<ui32>(crapString0.length()) - 32, &read));
    UTest(Equal, read, static_cast<ui32>(crapString0.length()) - 32);
    UTest(true, !MemOps::Compare(readBuf, crapString0.data() + 32, crapString0.length() - 32));
    UTest(Equal, file.Size().Unwrap(), crapString0.length());
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), static_cast<i64>(crapString0.length()));
    UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), 0LL);

    UTest(true, file.Offset(FileOffsetMode::FromBegin, 64));
    UTest(Equal, file.Offset(FileOffsetMode::FromBegin).Unwrap(), 64LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromCurrent).Unwrap(), 0LL);
    UTest(Equal, file.Offset(FileOffsetMode::FromEnd).Unwrap(), -(static_cast<i64>(crapString0.length()) - 64));
    UTest(Equal, file.Size().Unwrap(), crapString0.length());
    UTest(true, file.Write(crapString1.data(), static_cast<ui32>(crapString1.length()), &written));
    UTest(Equal, written, static_cast<ui32>(crapString1.length()));

    UTest(true, file.Offset(FileOffsetMode::FromBegin, 0));
    UTest(Equal, file.Size().Unwrap(), crapString0.length());
    crapString0.replace(crapString0.begin() + 64, crapString0.begin() + 64 + crapString1.length(), crapString1.data(), crapString1.length());
    UTest(true, file.Read(readBuf, static_cast<ui32>(crapString0.length()) + 999, &read)); // read outside the file, must truncate the requested size
    UTest(Equal, read, static_cast<ui32>(crapString0.length()));
    UTest(true, !MemOps::Compare(readBuf, crapString0.data(), crapString0.length()));
}

static void TestFileToMemoryStream()
{
    MemoryStreamAllocator<> memoryStream;
    
    Error<> fileError = DefaultError::Ok();
    MemoryStreamFile file = MemoryStreamFile(memoryStream, FileProcModes::Write, 0, &fileError);
    UTest(true, !fileError && file);
    FileWrite(file);

	FileTypeTest<MemoryStreamFile>(file);

    file = MemoryStreamFile(memoryStream, FileProcModes::Read, 0, &fileError);
    UTest(true, !fileError && file);
    FileRead(file);

    file = MemoryStreamFile(memoryStream, FileProcModes::Read, 2, &fileError);
    UTest(true, !fileError && file);
    FileReadOffsetted(file, 2);

    file = MemoryStreamFile(memoryStream, FileProcModes::Write, uiw_max, &fileError);
    UTest(true, !fileError && file);
    FileAppendWrite(file);

    file = MemoryStreamFile(memoryStream, FileProcModes::Read, 0, &fileError);
    UTest(true, !fileError && file);
    FileAppendRead(file);

    file = MemoryStreamFile(memoryStream, FileProcModes::Read.Combined(FileProcModes::Write), 0, &fileError);
    UTest(true, !fileError && file);
    FileWriteRead(file);

    UnitTestsLogger::Message("finished file memory stream tests\n");
}

template <typename T> static void TestFile(const FilePath &folderForTests)
{
    auto internalTest = [folderForTests](ui32 bufferSize, std::function<File::bufferType()> allocBufFunc)
    {
		FilePath path = folderForTests / TSTR("TestFile.txt");

        Error<> fileError = DefaultError::Ok();
        T file = T(path, FileOpenMode::CreateAlways, FileProcModes::Write, 0, FileCacheModes::Default, FileShareModes::None, &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileWrite(file);
		// TODO: fix
		//if constexpr (std::is_same_v<T, File>)
		//{
		//	UTest(true, FileSystem::IsPointToTheSameFile(file.PNN().Unwrap(), path));
		//}
        file.Close();

		FileTypeTest<T>(file);

        file = T(path, FileOpenMode::OpenExisting, FileProcModes::Read, 0, FileCacheModes::Default, FileShareModes::Read, &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileRead(file);
        file.Close();

        file = T(path, FileOpenMode::OpenExisting, FileProcModes::Read, 2, FileCacheModes::Default, FileShareModes::Read, &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileReadOffsetted(file, 2);
        file.Close();

        file = T(path, FileOpenMode::OpenExisting, FileProcModes::Write, uiw_max, FileCacheModes::Default, FileShareModes::None, &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileAppendWrite(file);
        file.Close();

        file = T(path, FileOpenMode::OpenExisting, FileProcModes::Read, 0, FileCacheModes::Default, FileShareModes::Read, &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileAppendRead(file);
        file.Close();

        file = T(path, FileOpenMode::CreateAlways, FileProcModes::Read.Combined(FileProcModes::Write), 0, FileCacheModes::Default, FileShareModes::Read.Combined(FileShareModes::Write), &fileError);
        UTest(true, !fileError && file);
        file.Buffer(bufferSize, allocBufFunc());
        FileWriteRead(file);
    };

	auto allocNullBuff = []() -> File::bufferType
	{
		return {nullptr, nullptr};
	};

	auto allocSizedBuff = [](uiw size) -> File::bufferType
	{
		return {new std::byte[10], [](std::byte *ptr) { delete[] ptr; }};
	};

    internalTest(0, allocNullBuff);
    internalTest(10, allocNullBuff);
    internalTest(2, allocNullBuff);
    internalTest(10, std::bind(allocSizedBuff, 10));

    UnitTestsLogger::Message("finished template file tests for %s\n", T::GetTypeName().data());
}

template <typename T> static void TestFileSharing(const FilePath &folderForTests)
{
    Error<> fileError = DefaultError::Ok();
    T file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::CreateAlways, FileProcModes::Write, 0, FileCacheModes::Default, FileShareModes::None, &fileError);
    UTest(true, !fileError && file);

    T file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcModes::Read, 0, FileCacheModes::Default, FileShareModes::None, &fileError);
    UTest(true, fileError && !file2);

    file.Close();
    file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcModes::Read, 0, FileCacheModes::Default, FileShareModes::Read, &fileError);
    UTest(true, !fileError && file2);

    file2.Close();
    file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcModes::Read, 0, FileCacheModes::Default, FileShareModes::Write, &fileError);
    UTest(true, !fileError && file);

    UnitTestsLogger::Message("finished template file sharing tests for %s\n", T::GetTypeName().data());
}

static void TestFileSystem(const FilePath &folderForTests)
{
    UTest(false, FileSystem::CreateNewFolder(folderForTests, TSTR("folder"), true));
    UTest(true, FileSystem::CreateNewFolder(folderForTests, TSTR("folder"), false));
    FilePath dirTestPath = folderForTests / TSTR("folder");
    UTest(Equal, FileSystem::Classify(dirTestPath).Unwrap(), FileSystem::ObjectType::Folder);
    UTest(NotEqual, FileSystem::Classify(dirTestPath).Unwrap(), FileSystem::ObjectType::File);
    UTest(true, FileSystem::IsFolderEmpty(dirTestPath).Unwrap());
    UTest(false, FileSystem::Remove(dirTestPath));

    UTest(false, FileSystem::CreateNewFolder(dirTestPath, {}, false)); // for some reason creating this folder will make folderForTests NotFound
    UTest(Equal, FileSystem::IsFolderEmpty(dirTestPath).Unwrap(), true);
    Error<> fileError = DefaultError::Ok();
    FilePath tempFilePath = dirTestPath / TSTR("tempFile.txt");
    UTest(Equal, FileSystem::Classify(tempFilePath).GetError(), DefaultError::NotFound());
    StandardFile tempFile(tempFilePath, FileOpenMode::CreateAlways, FileProcModes::Write, 0, FileCacheModes::Default, FileShareModes::None, &fileError);
	UTest(true, tempFile);
	tempFile.Close();
    UTest(false, fileError);
    UTest(Equal, FileSystem::IsFolderEmpty(dirTestPath).Unwrap(), false);
    UTest(Equal, FileSystem::Classify(tempFilePath).Unwrap(), FileSystem::ObjectType::File);
    FilePath tempFile2Path = dirTestPath / TSTR("tempFile2.txt");
    UTest(false, FileSystem::CopyTo(tempFilePath, tempFile2Path, false));
    FilePath tempFileRenamedPath = dirTestPath / TSTR("tempFileRenamed.txt");
    UTest(false, FileSystem::MoveTo(tempFilePath, tempFileRenamedPath, false));
    UTest(Equal, FileSystem::Classify(tempFilePath).GetError(), DefaultError::NotFound());
    UTest(Equal, FileSystem::Classify(tempFileRenamedPath).Unwrap(), FileSystem::ObjectType::File);
    //UTest(false, FileSystem::CopyTo(dirTestPath, dirTestPath / TSTR("foldierCopy"), false));

    UTest(Equal, FileSystem::IsReadOnly(tempFileRenamedPath).Unwrap(), false);
    UTest(false, FileSystem::IsReadOnly(tempFileRenamedPath, true));
    UTest(Equal, FileSystem::IsReadOnly(tempFileRenamedPath).Unwrap(), true);
    UTest(false, FileSystem::IsReadOnly(tempFileRenamedPath, false));
    UTest(Equal, FileSystem::IsReadOnly(tempFileRenamedPath).Unwrap(), false);

    UnitTestsLogger::Message("finished filesystem tests\n");
}

static void TestMemoryMappedFile(const FilePath &folderForTests)
{
    std::string crapString(256, '\0');
    for (char &ch : crapString)
    {
        ch = 32 + rand() % 96;
    }

    char tempBuf[256];

    File file = File(folderForTests / TSTR("memMapped.txt"), FileOpenMode::CreateAlways, FileProcModes::Read.Combined(FileProcModes::Write));
    UTest(true, file);
    UTest(true, file.Write(crapString.data(), static_cast<ui32>(crapString.length())));

    Error<> error = DefaultError::Ok();
    MemoryMappedFile mapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mapping);
    UTest(Equal, mapping.Size(), crapString.length());
    UTest(Equal, mapping.IsWritable(), true);
    UTest(true, !MemOps::Compare(crapString.data(), mapping.Memory(), crapString.size()));

    MemoryMappedFile mappingDuplicate = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mappingDuplicate);
    UTest(NotEqual, mapping.Memory(), mappingDuplicate.Memory());

    MemoryMappedFile mappingOffsetted = MemoryMappedFile(file, 2, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mappingOffsetted);

    std::reverse(crapString.begin(), crapString.end());
    UTest(false, !MemOps::Compare(crapString.data(), mapping.Memory(), crapString.size()));

    UTest(true, file.Offset(FileOffsetMode::FromBegin, 0));
    UTest(true, file.Write(crapString.data(), static_cast<ui32>(crapString.length())));
    UTest(true, file.Flush());

    UTest(true, !MemOps::Compare(crapString.data(), mapping.Memory(), crapString.size()));
    std::reverse(crapString.begin(), crapString.end());
    UTest(false, !MemOps::Compare(crapString.data(), mapping.Memory(), crapString.size()));

    MemOps::Copy(mapping.Memory(), crapString.data(), crapString.length());
    mapping.Flush();
    UTest(true, file.Offset(FileOffsetMode::FromBegin, 0));
    UTest(true, file.Read(tempBuf, static_cast<ui32>(crapString.length())));
    UTest(true, !MemOps::Compare(crapString.data(), tempBuf, crapString.length()));

    file.Close();
    UTest(true, !MemOps::Compare(crapString.data(), mapping.Memory(), crapString.size()));

    UTest(true, !MemOps::Compare(mapping.Memory(), mappingDuplicate.Memory(), crapString.size())); // different mapping should be coherent

    UTest(true, !MemOps::Compare(mapping.Memory() + 2, mappingOffsetted.Memory(), crapString.size() - 2));

    file = File(folderForTests / TSTR("memMapped2.txt"), FileOpenMode::CreateAlways, FileProcModes::Read.Combined(FileProcModes::Write));
    UTest(true, file);
    FileWrite(file);
    mapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mapping);
    auto memoryStream = mapping.ToMemoryStream();
    MemoryStreamFile memoryStreamFile = MemoryStreamFile(memoryStream, FileProcModes::Read);
    UTest(true, memoryStreamFile);
    FileRead(memoryStreamFile);

    file = File(folderForTests / TSTR("appendTest.txt"), FileOpenMode::CreateAlways, FileProcModes::Write);
    UTest(true, file);
    std::string_view invisiblePartStr = "invisible part";
    UTest(true, file.Write(invisiblePartStr.data(), static_cast<ui32>(invisiblePartStr.length())));
    file.Close();
    file = File(folderForTests / TSTR("appendTest.txt"), FileOpenMode::OpenExisting, FileProcModes::Write.Combined(FileProcModes::Read), uiw_max);
    std::string_view currentPartStr = "current part";
    UTest(true, file.Write(currentPartStr.data(), static_cast<ui32>(currentPartStr.length())));

    MemoryMappedFile appendedFileMapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, appendedFileMapping);
    UTest(true, !MemOps::Compare(appendedFileMapping.Memory(), currentPartStr.data(), currentPartStr.size()));

    UnitTestsLogger::Message("finished memory mapped file tests\n");
}

static void TimeMomentTests()
{
    using namespace std::chrono_literals;
    TimeMoment moment;
    UTest(false, moment.HasValue());
    moment = TimeMoment::Now();
    UTest(true, moment.HasValue());
    std::this_thread::sleep_for(10ms);
    TimeMoment moment2 = TimeMoment::Now();
    UTest(LeftLesser, moment, moment2);
    UTest(LeftGreater, moment2, moment);
    UTest(LeftGreater, moment + 0.5_s, moment2);
    UTest(LeftGreater, moment, moment2 - 0.5_s);
    TimeDifference diff = moment2 - moment;
    f32 diffSeconds = diff.ToSec();
    UTest(LeftGreater, diffSeconds, 0.0001f);
    UTest(LeftGreaterEqual, TimeMoment::Now(), moment2);
    UTest(LeftGreaterEqual, TimeMoment::Now(), moment);
    UTest(true, EqualsWithEpsilon(((moment + diff) - moment2).ToSec(), 0.0f, 0.0001f));
    UTest(true, EqualsWithEpsilon(((moment2 - diff) - moment).ToSec(), 0.0f, 0.0001f));
    TimeDifference refDiff = 0.5_s;
    auto testWithRef = [](TimeDifference refDiff, i64 timeUSec)
    {
        UTest(true, EqualsWithEpsilon(refDiff.ToSec(), timeUSec / (1'000'000.0f), 0.00001f));
        UTest(true, EqualsWithEpsilon(refDiff.ToMSec(), timeUSec / (1'000.0f), 0.01f));
        UTest(true, EqualsWithEpsilon(refDiff.ToUSec(), static_cast<f32>(timeUSec), 10.0f));
        UTest(true, EqualsWithEpsilon(refDiff.ToSec_f64(), timeUSec / (1'000'000.0), 0.00001));
        UTest(true, EqualsWithEpsilon(refDiff.ToMSec_f64(), timeUSec / (1'000.0), 0.01));
        UTest(true, EqualsWithEpsilon(refDiff.ToUSec_f64(), static_cast<f64>(timeUSec), 10.0));
        UTest(true, refDiff.ToSec_i32() == timeUSec / 1'000'000);
        UTest(true, abs(refDiff.ToMSec_i32() - timeUSec / 1'000) <= 1);
        UTest(true, abs(refDiff.ToUSec_i32() - timeUSec) <= 10);
        UTest(true, refDiff.ToSec_i64() == timeUSec / 1'000'000);
        UTest(true, abs(refDiff.ToMSec_i64() - timeUSec / 1'000) <= 1);
        UTest(true, abs(refDiff.ToUSec_i64() - timeUSec) <= 10);
    };
    testWithRef(0.0083333333333333333333333_m, 500'000);
    testWithRef(0.5_s, 500'000);
    testWithRef(500.0_ms, 500'000);
    testWithRef(500'000.0_us, 500'000);
    testWithRef(500_ms, 500'000);
    testWithRef(500'000_us, 500'000);
    testWithRef(2_s, 2'000'000);
    testWithRef(-2_s, -2'000'000);
    UTest(LeftLesser, TimeDifference(0.2_s), TimeDifference(0.5_s));

    UnitTestsLogger::Message("finished time moment tests\n");
}

static void DataHolderTests()
{
    using Holder64 = DataHolder<64>;
    using Holder16 = DataHolder<16>;

    struct NonCopyable
    {
        i32 _value = 17;
        i32 _moveCalledTimes = 0;

        NonCopyable() = default;
        NonCopyable(NonCopyable &&) noexcept
        {
            ++_moveCalledTimes;
        }
        NonCopyable &operator = (NonCopyable &&) noexcept
        {
            ++_moveCalledTimes;
            return *this;
        }
    };

    struct NonCopyable2 : NonCopyable
    {
		ui8 _crap[56]{};
    };

    struct NonCopyable3 : NonCopyable2
    {
		i32 _extra0{}, _extra1{};
    };

    Holder64 holder = Holder64(NonCopyable());
    UTest(true, holder.IsPlacedLocally<NonCopyable>());
    UTest(true, holder.IsPlacedLocally<NonCopyable2>());
    UTest(false, holder.IsPlacedLocally<NonCopyable3>());

    Holder64 holder2 = std::move(holder);

    NonCopyable &data = holder2.Get<NonCopyable>();
    UTest(Equal, data._value, 17);
    UTest(LeftGreater, data._moveCalledTimes, 0);
    holder2.Destroy<NonCopyable>();

    Holder16 holder3 = Holder16(NonCopyable3());
    UTest(false, holder3.IsPlacedLocally<NonCopyable3>());
    auto &dataInH3 = holder3.Get<NonCopyable3>();
    UTest(Equal, dataInH3._value, 17);
    UTest(LeftGreaterEqual, dataInH3._moveCalledTimes, 0);
    Holder16 holder4 = std::move(holder3);
    NonCopyable3 &data2 = holder4.Get<NonCopyable3>();
    UTest(Equal, data2._value, 17);
    UTest(LeftGreater, data2._moveCalledTimes, 0);
    holder4.Destroy<NonCopyable>();

    UnitTestsLogger::Message("finished data holder tests\n");
}

static void MemoryStreamTests()
{
    std::string_view test0 = "Luna";
    std::string_view test1 = "Celestia";

    auto checkContent = [test0, test1](IMemoryStream &ms)
    {
        UTest(true, ms.IsReadable());
        UTest(Equal, ms.Size(), test0.length() + test1.length());
        UTest(true, !MemOps::Compare(ms.CMemory(), test0.data(), test0.length()));
        UTest(true, !MemOps::Compare(ms.CMemory() + test0.length(), test1.data(), test1.length()));
    };

    auto writeAndCheck = [test0, test1, checkContent](IMemoryStream &ms)
    {
        UTest(true, ms.IsReadable());
        UTest(Equal, ms.Resize(test0.length()), test0.length());
        MemOps::Copy(ms.Memory(), test0.data(), test0.length());
        UTest(Equal, ms.Resize(ms.Size() + test1.size()), test0.length() + test1.length());
		MemOps::Copy(ms.Memory() + test0.length(), test1.data(), test1.size());
        checkContent(ms);
    };

    MemoryStreamFixed<128> fixedTest;
    writeAndCheck(fixedTest);

    MemoryStreamAllocator<> allocatorTest;
    writeAndCheck(allocatorTest);

    char buf[128];
    MemoryStreamFixedExternal externalTest = MemoryStreamFixedExternal(buf, CountOf(buf));
    writeAndCheck(externalTest);

    struct HolderTestData
    {
        char str[13] = "LunaCelestia";
	} holderTestData{};

    using memoryStreamType = MemoryStreamFromDataHolder<32>;

    using holderType = DataHolder<memoryStreamType::localSize, memoryStreamType::localAlignment>;
    holderType data = holderType(holderTestData);

    auto holderTestDataProvide = [](const holderType &data) -> const std::byte *
    {
        return reinterpret_cast<const std::byte *>(&data.Get<HolderTestData>().str);
    };

    auto dataHolderMS = memoryStreamType::New<HolderTestData>(std::move(data), test0.length() + test1.length(), holderTestDataProvide);
    checkContent(dataHolderMS);

    auto dataHolderMSMovedInfo = std::move(dataHolderMS);
    checkContent(dataHolderMSMovedInfo);

    {
        std::vector<memoryStreamType> streams;

        for (uiw index = 0; index < 25; ++index)
        {
            holderType tempData = holderType(holderTestData);
            auto stream = memoryStreamType::New<HolderTestData>(std::move(tempData), test0.length() + test1.length(), holderTestDataProvide);
            streams.push_back(std::move(stream));
        }

        streams.erase(streams.begin() + 5);

        std::exchange(streams, {});
    }

    UnitTestsLogger::Message("finished memory stream tests\n");
}

static void FunctionInfoTests()
{
    using test0 = FunctionInfo::Info<decltype(FunctionInfoTests)>;
    UTest(Equal, std::tuple_size_v<test0::args>, 0);
    UTest(true, std::is_same_v<test0::result, void>);

    struct local
    {
        static void f0(i32, f32);
        static i32 f1();
        f64 f2(ui8);
    };

    using test1 = FunctionInfo::Info<decltype(local::f0)>;
    UTest(true, std::is_same_v<std::tuple_element_t<0, test1::args>, i32>);
    UTest(true, std::is_same_v<std::tuple_element_t<1, test1::args>, f32>);
    UTest(true, std::is_same_v<test1::result, void>);

    using test2 = FunctionInfo::Info<decltype(local::f1)>;
    UTest(Equal, std::tuple_size_v<test2::args>, 0);
    UTest(true, std::is_same_v<test2::result, i32>);

    using test3 = FunctionInfo::Info<decltype(&local::f2)>;
    UTest(true, std::is_same_v<std::tuple_element_t<0, test3::args>, ui8>);
    UTest(true, std::is_same_v<test3::parentClass, local>);
    UTest(true, std::is_same_v<test3::result, f64>);

    using test4 = FunctionInfo::Info<void(*)()>;
    UTest(true, std::is_same_v<test4::result, void>);
    UTest(Equal, std::tuple_size_v<test4::args>, 0);

    UnitTestsLogger::Message("finished function info tests\n");
}

template <KeyCode... Codes> void TestKeys(bool(*func)(KeyCode key))
{
    auto test = [](bool(*func)(KeyCode key)) -> ui32
    {
        ui32 count = 0;
        for (ui32 index = 0; index < static_cast<ui32>(KeyCode::_size); ++index)
        {
            if (func(static_cast<KeyCode>(index)))
            {
                ++count;
            }
        }
        return count;
    };

    auto check = [&func](KeyCode code)
    {
        UTest(true, func(code));
    };

    (check(Codes), ...);

    UTest(Equal, test(func), sizeof...(Codes));
}

static void VirtualKeysTests()
{
    TestKeys<KeyCode::LAlt, KeyCode::RAlt>(VirtualKeys::IsAlt);
    TestKeys<KeyCode::LeftArrow, KeyCode::RightArrow, KeyCode::UpArrow, KeyCode::DownArrow>(VirtualKeys::IsArrowKey);
    TestKeys<KeyCode::A, KeyCode::B, KeyCode::C, KeyCode::D, KeyCode::E, KeyCode::F, KeyCode::G, KeyCode::H, KeyCode::I, KeyCode::J, KeyCode::K, KeyCode::L, KeyCode::M, KeyCode::N, KeyCode::O, KeyCode::P, KeyCode::Q, KeyCode::R, KeyCode::S, KeyCode::T, KeyCode::U, KeyCode::V, KeyCode::W, KeyCode::X, KeyCode::Y, KeyCode::Z>(VirtualKeys::IsLetter);
    TestKeys<KeyCode::F1, KeyCode::F2, KeyCode::F3, KeyCode::F4, KeyCode::F5, KeyCode::F6, KeyCode::F7, KeyCode::F8, KeyCode::F9, KeyCode::F10, KeyCode::F11, KeyCode::F12>(VirtualKeys::IsFKey);
    TestKeys<KeyCode::Digit0, KeyCode::Digit1, KeyCode::Digit2, KeyCode::Digit3, KeyCode::Digit4, KeyCode::Digit5, KeyCode::Digit6, KeyCode::Digit7, KeyCode::Digit8, KeyCode::Digit9>(VirtualKeys::IsDigit);
    TestKeys<KeyCode::NPad0, KeyCode::NPad1, KeyCode::NPad2, KeyCode::NPad3, KeyCode::NPad4, KeyCode::NPad5, KeyCode::NPad6, KeyCode::NPad7, KeyCode::NPad8, KeyCode::NPad9, KeyCode::NPadPlus, KeyCode::NPadMinus, KeyCode::NPadMul, KeyCode::NPadDiv>(VirtualKeys::IsNPadKey);
    TestKeys<KeyCode::NPad0, KeyCode::NPad1, KeyCode::NPad2, KeyCode::NPad3, KeyCode::NPad4, KeyCode::NPad5, KeyCode::NPad6, KeyCode::NPad7, KeyCode::NPad8, KeyCode::NPad9>(VirtualKeys::IsNPadDigit);
    TestKeys<KeyCode::NPad4, KeyCode::NPad8, KeyCode::NPad2, KeyCode::NPad6>(VirtualKeys::IsNPadArrow);
    TestKeys<KeyCode::LEnter, KeyCode::REnter>(VirtualKeys::IsEnter);
    TestKeys<KeyCode::LShift, KeyCode::RShift>(VirtualKeys::IsShift);
    TestKeys<KeyCode::LSystem, KeyCode::RSystem>(VirtualKeys::IsSystem);
    TestKeys<KeyCode::LDelete, KeyCode::RDelete>(VirtualKeys::IsDelete);
    TestKeys<KeyCode::MButton0, KeyCode::MButton1, KeyCode::MButton2, KeyCode::MButton3, KeyCode::MButton4, KeyCode::MButton5, KeyCode::MButton6>(VirtualKeys::IsMouseButton);

    UnitTestsLogger::Message("finished virtual keys tests\n");
}

static void TypeTests()
{
	auto checkType = [](const auto &value)
	{
		using T = std::remove_reference_t<decltype(value)>;
		UTest(true, value.Type());
		UTest(Equal, value.Type(), T::GetTypeId());
		UTest(NotEqual, value.GetTypeName(), std::string_view{});
		UTest(Equal, value.GetTypeName(), T::GetTypeName());
	}; 
	
	using memoryStreamType = MemoryStreamFromDataHolder<32>;
	using holderType = DataHolder<32>;
	holderType data = holderType(int());

	auto dataHolderMS = memoryStreamType::New<int>(std::move(data), sizeof(int));

	checkType(File());
	checkType(StandardFile());
	checkType(MemoryStreamFile());
	checkType(MemoryStreamAllocator<>());
	checkType(MemoryStreamFixed<100>());
	checkType(MemoryStreamFixedExternal());
	checkType(dataHolderMS);

	UnitTestsLogger::Message("finished type matching tests\n");
}

static void PrintSystemInfo()
{
	auto cacheInfo = SystemInfo::AcquireCacheInfo();

	UnitTestsLogger::Message("System info:\n");
	UnitTestsLogger::Message("  Logical CPU cores %u\n", SystemInfo::LogicalCPUCores());
	UnitTestsLogger::Message("  Physical CPU cores %u\n", SystemInfo::PhysicalCPUCores());
	for (uiw index = 0; index < cacheInfo.second; ++index)
	{
		auto info = cacheInfo.first[index];
		auto typeToString = [&info]
		{
			switch (info.type)
			{
			case SystemInfo::CacheInfo::Type::Data:
				return "Data";
			case SystemInfo::CacheInfo::Type::Instruction:
				return "Instruction";
			case SystemInfo::CacheInfo::Type::Trace:
				return "Trace";
			case SystemInfo::CacheInfo::Type::Unified:
				return "Unified";
			}
			UNREACHABLE;
			return "";
		};
		auto sizeToString = [&info]
		{
			uiw size = info.size;
			const char *label = " B";
			if (size / 1024)
			{
				size /= 1024;
				label = " KB";
				if (size / 1024)
				{
					size /= 1024;
					label = " MB";
				}
			}
			return std::to_string(size) + label;
		};
		UnitTestsLogger::Message("    %s\n", typeToString());
		UnitTestsLogger::Message("      Level %u\n", info.level);
		UnitTestsLogger::Message("      Line size %u\n", info.lineSize);
		UnitTestsLogger::Message("      Associativity %u\n", info.associativity);
		UnitTestsLogger::Message("      Size %s\n", sizeToString().c_str());
		if (info.isPerCore.has_value())
		{
			UnitTestsLogger::Message("      Per core %s\n", *info.isPerCore ? "true" : "false");
		}
	}
	UnitTestsLogger::Message("  Memory allocation alignment %u\n", static_cast<ui32>(SystemInfo::AllocationAlignment()));
    UnitTestsLogger::Message("  Memory page size %u\n", static_cast<ui32>(VirtualMemory::PageSize()));
	const char *arch = nullptr;
	switch (SystemInfo::CPUArchitecture())
	{
	case SystemInfo::Arch::ARM:
		arch = "ARM";
        break;
	case SystemInfo::Arch::ARMT:
		arch = "ARMT";
		break;
	case SystemInfo::Arch::ARM64:
		arch = "ARM64";
		break;
	case SystemInfo::Arch::Unwnown:
		arch = "Unknown";
		break;
	case SystemInfo::Arch::x64:
		arch = "x64";
		break;
	case SystemInfo::Arch::x86:
		arch = "x86";
		break;
	case SystemInfo::Arch::Emscripten:
		arch = "Emscripten";
		break;
	}
	UnitTestsLogger::Message("  CPU architecture %s\n", arch);
    const char *platform = nullptr;
    switch (SystemInfo::CurrentPlatform())
    {
    case SystemInfo::Platform::Android:
        platform = "Android";
        break;
    case SystemInfo::Platform::Emscripten:
        platform = "Emscripten";
        break;
    case SystemInfo::Platform::iOS:
        platform = "iOS";
        break;
    case SystemInfo::Platform::Linux:
        platform = "Linux";
        break;
    case SystemInfo::Platform::macOS:
        platform = "macOS";
        break;
    case SystemInfo::Platform::Unknown:
        platform = "Unknown";
        break;
    case SystemInfo::Platform::Windows:
        platform = "Windows";
        break;
    }
    UnitTestsLogger::Message("  Platform %s\n", platform);
    UnitTestsLogger::Message("Finished printing system info\n");
}

static void DoTests(int argc, char **argv)
{
	const char *buildType = "Release build";
	#if defined(DEBUG) || defined(_DEBUG)
		buildType = "Debug build";
	#endif

    UnitTestsLogger::Message("---Starting Tests for %s---\n", buildType);

    std::string filesTestFolder = "FileTestsFolder";
    if (argc >= 2)
    {
        filesTestFolder = argv[1];
        if (filesTestFolder.size() && filesTestFolder.back() != '/')
        {
            filesTestFolder.push_back('/');
        }
        filesTestFolder += "FileTestsFolder";
    }

    FilePath folderForTests = FilePath::FromChar(filesTestFolder);
    UTest(false, FileSystem::CreateNewFolder(folderForTests, {}, true));

    MiscTests();
	MemOpsTests();
	TupleHelpersTests();
    CompileTimeSortingTests();
    CompileTimeStringsTests();
    EnumCombinableTests();
	TypeIdentifiableTests();
	HashFuncsTest();
    IntegerPropertiesTest();
    SetBitTests();
    SignificantBitTests();
    ChangeEndiannessTests();
    RotateBitsTests();
    ErrorTests();
    ResultTests();
    VirtualMemoryTests();
    AllocatorsTests();
    FilePathTests();
    TestFileToMemoryStream();
    TestFile<StandardFile>(folderForTests);
    TestFile<File>(folderForTests);
#ifndef PLATFORM_POSIX
    TestFileSharing<StandardFile>(folderForTests);
    TestFileSharing<File>(folderForTests);
#endif
    TestFileSystem(folderForTests);
    TestMemoryMappedFile(folderForTests);
    TimeMomentTests();
    DataHolderTests();
    MemoryStreamTests();
    FunctionInfoTests();
    VirtualKeysTests();
	TypeTests();
    MathLibTests();
    UniqueIdManagerTests();
	LoggerTests();
    MTTests();
	PrintSystemInfo();

    Error<> folderForTestsRemove = FileSystem::Remove(folderForTests);
    UTest(false, folderForTestsRemove);

    UnitTestsLogger::Message("---Finished All Tests---\n");
}

int main(int argc, char **argv)
{
#if defined(_MSC_VER) && defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF/* | _CRTDBG_CHECK_EVERY_1024_DF*/);
#endif

    StdLib::Initialization::Initialize({});

#ifdef PLATFORM_WINDOWS
	auto console = NativeConsole(true);
	if (console)
	{
		ui32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
		ui32 screenHeight = GetSystemMetrics(SM_CYSCREEN);
		ui32 preferredWidth = screenWidth / 4;
		ui32 preferredHeight = screenHeight / 5;
		ui32 width = std::max(preferredWidth, 750u);
		ui32 height = std::max(preferredHeight, 450u);
		console.
			BufferSize(std::nullopt, 150).
			Size(width, height).
			Position((static_cast<i32>(screenWidth) - static_cast<i32>(width)) / 2, (static_cast<i32>(screenHeight) - static_cast<i32>(height)) / 2).
			SnapToWindowSize(true).
			Modes(NativeConsole::ModeValues::EnableMouseSelection);
	}
#endif

    DoTests(argc, argv);

    //UniqueIdManagerBenchmark();

    UnitTestsLogger::Message("---Finished Everything---\n");

#ifdef PLATFORM_WINDOWS


    system("pause");
#endif

	return 0;
}