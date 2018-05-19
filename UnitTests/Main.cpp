#include <StdPlatformAbstractionLib.hpp>
#include <FileToMemoryStream.hpp>
#include <FileToCFile.hpp>
#include <FileSystem.hpp>
#include <File.hpp>

#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
#endif

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

enum UnitTestType
{
    LeftLess,
    LeftLessEqual,
    Equal,
    LeftGreaterEqual,
    LeftGreater,
    NotEqual
};

template <UnitTestType testType, typename T, typename E> void UnitTest(T &&left, E &&right)
{
    if constexpr (testType == LeftLess) ASSUME(left < right);
    else if constexpr (testType == LeftLessEqual) ASSUME(left <= right);
    else if constexpr (testType == Equal) ASSUME(left == right);
    else if constexpr (testType == LeftGreaterEqual) ASSUME(left >= right);
    else if constexpr (testType == LeftGreater) ASSUME(left > right);
    else if constexpr (testType == NotEqual) ASSUME(left != right);
}

template <bool isTrue, typename T> void UnitTest(T &&value)
{
    if constexpr (isTrue)
    {
        ASSUME(value);
    }
    else
    {
        ASSUME(!value);
    }
}

static void SetBitTests()
{
    ui32 value = 0;
    value = SetBit(value, 14u, true);
    UnitTest<Equal>(value, 16384u);
    UnitTest<true>(IsBitSet(value, 14u));
    UnitTest<false>(IsBitSet(value, 13u));
    value = SetBit(value, 14u, false);
    UnitTest<Equal>(value, 0u);

    PRINTLOG("finished set bit tests\n");
}

static void SignificantBitTests()
{
    ui64 ui64value = 16384;
    UnitTest<Equal>(MostSignificantNonZeroBit(ui64value), 14u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(ui64value), 14u);
    ui64value = 1ULL << 63;
    UnitTest<Equal>(MostSignificantNonZeroBit(ui64value), 63u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(ui64value), 63u);

    i64 i64value = i64_min;
    UnitTest<Equal>(MostSignificantNonZeroBit(i64value), 63u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(i64value), 63u);
    i64value = 15;
    UnitTest<Equal>(MostSignificantNonZeroBit(i64value), 3u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(i64value), 0u);

    ui8 ui8value = 128;
    UnitTest<Equal>(MostSignificantNonZeroBit(ui8value), 7u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(ui8value), 7u);
    ui8value = 15;
    UnitTest<Equal>(MostSignificantNonZeroBit(ui8value), 3u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(ui8value), 0u);

    i8 i8value = -128;
    UnitTest<Equal>(MostSignificantNonZeroBit(i8value), 7u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(i8value), 7u);
    i8value = 15;
    UnitTest<Equal>(MostSignificantNonZeroBit(i8value), 3u);
    UnitTest<Equal>(LeastSignificantNonZeroBit(i8value), 0u);

    PRINTLOG("finished significant bit tests\n");
}

static void ChangeEndiannessTests()
{
    ui64 ui64value = 0xFF'00'00'00'FF'00'00'00ULL;
    UnitTest<Equal>(ChangeEndianness(ui64value), 0x00'00'00'FF'00'00'00'FFULL);
    ui32 ui32value = 0xFF'00'00'00;
    UnitTest<Equal>(ChangeEndianness(ui32value), 0x00'00'00'FFU);
    ui16 ui16value = 0xFF'00;
    UnitTest<Equal>(ChangeEndianness(ui16value), 0x00'FF);
    ui8 ui8value = 0xFF;
    UnitTest<Equal>(ChangeEndianness(ui8value), 0xFF);

    PRINTLOG("finished change endianness tests\n");
}

static void RotateBitsTests()
{
    ui64 ui64value = 0b001;
    ui64value = RotateBitsRight(ui64value, 2);
    UnitTest<Equal>(ui64value, 0x40'00'00'00'00'00'00'00ULL);
    ui64value = RotateBitsLeft(ui64value, 2);
    UnitTest<Equal>(ui64value, 0b001);
    ui32 ui32value = 0b001;
    ui32value = RotateBitsRight(ui32value, 2);
    UnitTest<Equal>(ui32value, 0x40'00'00'00U);
    ui32value = RotateBitsLeft(ui32value, 2);
    UnitTest<Equal>(ui32value, 0b001);
    ui16 ui16value = 0b001;
    ui16value = RotateBitsRight(ui16value, 2);
    UnitTest<Equal>(ui16value, 0x40'00);
    ui16value = RotateBitsLeft(ui16value, 2);
    UnitTest<Equal>(ui16value, 0b001);
    ui8 ui8value = 0b001;
    ui8value = RotateBitsRight(ui8value, 2);
    UnitTest<Equal>(ui8value, 0x40);
    ui8value = RotateBitsLeft(ui8value, 2);
    UnitTest<Equal>(ui8value, 0b001);

    PRINTLOG("finished rotate bits tests\n");
}

static void ErrorTests()
{
    auto okError = DefaultError::Ok();
    UnitTest<true>(okError.IsOk());
    UnitTest<false>(okError);
    UnitTest<Equal>(okError, DefaultError::Ok());
    UnitTest<NotEqual>(okError, DefaultError::UnknownError());

    auto unknownFormatWithAttachment = Error<std::string>(DefaultError::UnknownFormat(), "MPEG");
    UnitTest<false>(unknownFormatWithAttachment.IsOk());
    UnitTest<true>(unknownFormatWithAttachment);
    UnitTest<Equal>(unknownFormatWithAttachment, DefaultError::UnknownFormat());
    UnitTest<Equal>(unknownFormatWithAttachment.Attachment(), "MPEG");

    auto busyWithCustomDescription = Error<void, std::string>(DefaultError::Busy(), "Bus was handling "s + std::to_string(284) + " other requrests"s);
    UnitTest<false>(busyWithCustomDescription.IsOk());
    UnitTest<true>(busyWithCustomDescription);
    UnitTest<Equal>(busyWithCustomDescription, DefaultError::Busy());
    UnitTest<Equal>(busyWithCustomDescription.Description(), "Bus was handling "s + std::to_string(284) + " other requrests"s);

    struct Canceller {};
    auto canceller = std::make_shared<Canceller>();
    auto cancelledWithAttachmentAndCustomDescription = Error<std::shared_ptr<Canceller>, std::string>(DefaultError::Cancelled(), canceller, "Cancelling reason: "s + "interrupted by user"s);
    UnitTest<false>(cancelledWithAttachmentAndCustomDescription.IsOk());
    UnitTest<true>(cancelledWithAttachmentAndCustomDescription);
    UnitTest<Equal>(cancelledWithAttachmentAndCustomDescription, DefaultError::Cancelled());
    UnitTest<true>(AreSharedPointersEqual(cancelledWithAttachmentAndCustomDescription.Attachment(), canceller));
    UnitTest<Equal>(cancelledWithAttachmentAndCustomDescription.Description(), "Cancelling reason: "s + "interrupted by user"s);

    cancelledWithAttachmentAndCustomDescription = decltype(cancelledWithAttachmentAndCustomDescription)::FromDefaultError(DefaultError::Ok());
    UnitTest<true>(cancelledWithAttachmentAndCustomDescription.IsOk());
    UnitTest<false>(cancelledWithAttachmentAndCustomDescription);
    UnitTest<Equal>(cancelledWithAttachmentAndCustomDescription, DefaultError::Ok());
    UnitTest<Equal>(cancelledWithAttachmentAndCustomDescription.Attachment(), std::shared_ptr<Canceller>());
    UnitTest<Equal>(cancelledWithAttachmentAndCustomDescription.Description(), "");

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
    UnitTest<false>(customErrorWithAttachmentAndCustomDescription.IsOk());
    UnitTest<true>(customErrorWithAttachmentAndCustomDescription);
    UnitTest<Equal>(customErrorWithAttachmentAndCustomDescription, createCustomErrorUnitTest());
    UnitTest<Equal>(customErrorWithAttachmentAndCustomDescription.Attachment(), -45);
    UnitTest<Equal>(customErrorWithAttachmentAndCustomDescription.Description(), "UnitTesting "s + std::to_string(-45));

    PRINTLOG("finished error tests\n");
}

static void ResultTests()
{
    Result<i32> intResult(43);
    UnitTest<true>(intResult.IsOk());
    UnitTest<true>(intResult);
    UnitTest<Equal>(intResult.GetError(), DefaultError::Ok());
    UnitTest<Equal>(intResult.Unwrap(), 43);

    PRINTLOG("finished result tests\n");
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
            UnitTest<Equal>(isExcepted, IsExpectingException); \
        }
#else
    #define EXCEPTION_CHECK(Code, IsExpectingException)
#endif

static void VirtualMemoryTests()
{
    void *memory = VirtualMemory::Reserve(999);
    UnitTest<true>(memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    auto commitError = VirtualMemory::Commit(memory, 500, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    UnitTest<true>(commitError.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    auto protection = VirtualMemory::PageModeGet(memory, VirtualMemory::PageSize());
#ifdef PLATFORM_WINDOWS
    UnitTest<Equal>(protection.Unwrap(), (VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write));
#else
    UnitTest<Equal>(protection.GetError(), DefaultError::Unsupported());
#endif

    auto protectionSetResult = VirtualMemory::PageModeSet(memory, VirtualMemory::PageSize(), VirtualMemory::PageMode::Read);
    UnitTest<true>(protectionSetResult.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    UnitTest<true>(VirtualMemory::Free(memory, 999));

    memory = VirtualMemory::Alloc(999, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    UnitTest<true>(memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    UnitTest<true>(VirtualMemory::Free(memory, 999));

    PRINTLOG("finished virtual memory tests\n");
}

static void AllocatorsTests()
{
    uiw blockSize = 0;

    void *memory = Allocator::MallocBased::Allocate(100);
    UnitTest<true>(memory);

#ifndef PLATFORM_ANDROID
    blockSize = Allocator::MallocBased::MemorySize(memory);
    UnitTest<LeftGreaterEqual>(blockSize, 100);
#endif

    bool expandResult = Allocator::MallocBased::ReallocateInplace(memory, 200);
    if (expandResult)
    {
        blockSize = Allocator::MallocBased::MemorySize(memory);
        UnitTest<LeftGreaterEqual>(blockSize, 200);
    }
    memory = Allocator::MallocBased::Reallocate(memory, 300);
    UnitTest<true>(memory);

#ifndef PLATFORM_ANDROID
    blockSize = Allocator::MallocBased::MemorySize(memory);
    UnitTest<LeftGreaterEqual>(blockSize, 300);
#endif

    Allocator::MallocBased::Free(memory);

    PRINTLOG("finished allocators tests\n");
}

void FilePathTests()
{
    FilePath path{TSTR("C:\\ext\\\\no.txt")};

    path.Normalize();
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/ext/no.txt"));
    auto fileName = path.FileNameView();
    UnitTest<Equal>(fileName, TSTR("no"));
    auto fileExt = path.ExtensionView();
    UnitTest<Equal>(fileExt, TSTR("txt"));
    path.ReplaceExtension(TSTR("bmp"));
    fileExt = path.ExtensionView();
    UnitTest<Equal>(fileExt, TSTR("bmp"));
    path.ReplaceFileName(TSTR("yes"));
    fileName = path.FileNameView();
    UnitTest<Equal>(fileName, TSTR("yes"));
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/ext/yes.bmp"));
    UnitTest<Equal>(path.FileNameExtView(), TSTR("yes.bmp"));
    path.RemoveLevel();
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/ext/"));
    path.RemoveLevel();
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/"));
    path += TSTR("folder");
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/folder"));
    UnitTest<Equal>(path.FileNameView(), TSTR("folder"));
    UnitTest<Equal>(path.FileNameExtView(), TSTR("folder"));
    UnitTest<true>(path.HasFileName());
    UnitTest<false>(path.ExtensionView());
    UnitTest<false>(path.HasExtension());
    path /= TSTR("file.ext");
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/folder/file.ext"));
    path.ReplaceFileNameExt(TSTR("new.fil"));
    UnitTest<Equal>(path.PlatformPath(), TSTR("C:/folder/new.fil"));

    PRINTLOG("finished FilePath tests\n");
}

static void FileWrite(IFile &file)
{
    UnitTest<Equal>((file.ProcMode() + FileProcMode::Write), file.ProcMode());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), 0);
    
    std::string_view str = "test0123456789 start";
    ui32 written = 0;
    UnitTest<true>(file.Write(str.data(), (ui32)str.length(), &written));
    UnitTest<Equal>(written, str.length());
    UnitTest<true>(file.Write(nullptr, 0, &written));
    UnitTest<Equal>(written, 0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), str.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), 0);
    
    if (file.IsSeekSupported())
    {
        i64 oldOffset = file.OffsetGet().Unwrap();
        UnitTest<true>(file.OffsetSet(FileOffsetMode::FromCurrent, -5));
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), str.length() - 5);
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -5);
        str = "trats";
        UnitTest<true>(file.Write(str.data(), (ui32)str.length(), &written));
        UnitTest<Equal>(written, str.length());
        UnitTest<Equal>(file.OffsetGet().Unwrap(), oldOffset);
    }

    auto startSize = "test0123456789 start"s.length();
    UnitTest<Equal>(file.SizeGet().Unwrap(), startSize);
    UnitTest<false>(file.SizeSet(startSize - 1));
    UnitTest<Equal>(file.SizeGet().Unwrap(), startSize - 1);
    UnitTest<false>(file.SizeSet(startSize + 1));
    UnitTest<Equal>(file.SizeGet().Unwrap(), startSize + 1);
    UnitTest<false>(file.SizeSet(startSize - 1));
    UnitTest<Equal>(file.SizeGet().Unwrap(), startSize - 1);
}

static void FileRead(IFile &file)
{
    UnitTest<Equal>(file.ProcMode(), FileProcMode::Read);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);

    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat" : "test0123456789 star";
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    UnitTest<true>(file.Read(target.data(), (ui32)target.length(), &read));
    UnitTest<Equal>(read, str.length());
    UnitTest<Equal>(str, target);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), str.length());
    UnitTest<true>(file.Read(nullptr, 0, &read));
    UnitTest<Equal>(read, 0);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), str.length());

    if (file.IsSeekSupported())
    {
        UnitTest<true>(file.OffsetSet(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        UnitTest<true>(file.Read(target.data(), (ui32)target.length(), &read));
        UnitTest<Equal>(read, str.length());
        UnitTest<Equal>(target, str);
    }
}

static void FileAppendWrite(IFile &file)
{
    UnitTest<Equal>((file.ProcMode() + FileProcMode::WriteAppend), file.ProcMode());
    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);
    std::string_view str = "9184";
    ui32 written = 0;
    UnitTest<true>(file.Write(str.data(), (ui32)str.length(), &written));
    UnitTest<Equal>(written, str.length());
    UnitTest<Equal>(file.OffsetGet().Unwrap(), str.length());
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
}

static void FileAppendRead(IFile &file)
{
    UnitTest<Equal>(file.ProcMode(), FileProcMode::Read);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);
    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat9184" : "test0123456789 star9184";
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
    std::string target(str.length(), '\0');
    ui32 read = 0;
    UnitTest<true>(file.Read(target.data(), target.length(), &read));
    UnitTest<Equal>(read, str.length());
    UnitTest<Equal>(target, str);
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

    ui32 written, read;

    char readBuf[256];

    UnitTest<Equal>((file.ProcMode() + FileProcMode::Write + FileProcMode::Read), file.ProcMode());

    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);
    UnitTest<true>(file.Write(crapString0.data(), (ui32)crapString0.length(), &written));
    UnitTest<Equal>(written, (ui32)crapString0.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)crapString0.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), (i64)0);
    UnitTest<Equal>(file.SizeGet().Unwrap(), crapString0.length());

    UnitTest<true>(file.OffsetSet(FileOffsetMode::FromBegin, (i64)32));
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)32);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -((i64)crapString0.length() - 32));
    UnitTest<true>(file.Read(readBuf, (ui32)crapString0.length() - 32, &read));
    UnitTest<Equal>(read, (ui32)crapString0.length() - 32);
    UnitTest<true>(!memcmp(readBuf, crapString0.data() + 32, crapString0.length() - 32));
    UnitTest<Equal>(file.SizeGet().Unwrap(), crapString0.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)crapString0.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), (i64)0);

    UnitTest<true>(file.OffsetSet(FileOffsetMode::FromBegin, 64));
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)64);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -(i64)(crapString0.length() - 64));
    UnitTest<Equal>(file.SizeGet().Unwrap(), crapString0.length());
    UnitTest<true>(file.Write(crapString1.data(), (ui32)crapString1.length(), &written));
    UnitTest<Equal>(written, (ui32)crapString1.length());

    UnitTest<true>(file.OffsetSet(FileOffsetMode::FromBegin, 0));
    UnitTest<Equal>(file.SizeGet().Unwrap(), crapString0.length());
    crapString0.replace(crapString0.begin() + 64, crapString0.begin() + 64 + crapString1.length(), crapString1.data(), crapString1.length());
    UnitTest<true>(file.Read(readBuf, (ui32)crapString0.length(), &read));
    UnitTest<Equal>(read, (ui32)crapString0.length());
    UnitTest<true>(!memcmp(readBuf, crapString0.data(), crapString0.length()));
}

static void TestFileToMemoryStream()
{
    MemoryStreamAllocator<> memoryStream;
    
    Error<> fileError = DefaultError::Ok();
    FileToMemoryStream file = FileToMemoryStream(memoryStream, FileProcMode::Write, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileRead(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::WriteAppend, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileAppendWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileAppendRead(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read + FileProcMode::Write, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileWriteRead(file);

    PRINTLOG("finished file memory stream tests\n");
}

template <typename T> void TestFile(const FilePath &folderForTests)
{
    auto internalTest = [folderForTests](ui32 bufferSize, File::bufferType &&buffer)
    {
        Error<> fileError = DefaultError::Ok();
        T file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
        UnitTest<true>(!fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileWrite(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
        UnitTest<true>(!fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileRead(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::WriteAppend, FileCacheMode::Default, FileShareMode::None, &fileError);
        UnitTest<true>(!fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileAppendWrite(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
        UnitTest<true>(!fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileAppendRead(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Read + FileProcMode::Write, FileCacheMode::Default, FileShareMode::Read + FileShareMode::Write, &fileError);
        UnitTest<true>(!fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileWriteRead(file);
    };

    internalTest(0, {nullptr, nullptr});
    internalTest(10, {nullptr, nullptr});
    internalTest(2, {nullptr, nullptr});
    internalTest(10, File::bufferType{new ui8[10], [](ui8 *ptr) { delete[] ptr; }});

    PRINTLOG("finished template file tests\n");
}

template <typename T> void TestFileSharing(const FilePath &folderForTests)
{
    /*Error<> fileError = DefaultError::Ok();
    T file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());

    T file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::None, &fileError);
    UnitTest<true>(fileError && !file2.IsOpened());

    file.Close();
    file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
    UnitTest<true>(!fileError && file2.IsOpened());

    file2.Close();
    file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Write, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());*/

    PRINTLOG("finished template file sharing tests\n");
}

static void TestFiles(const FilePath &folderForTests)
{
    UnitTest<false>(FileSystem::CreateNewFolder(folderForTests, TSTR("folder"), true));
    UnitTest<true>(FileSystem::CreateNewFolder(folderForTests, TSTR("folder"), false));
    FilePath dirTestPath = folderForTests / TSTR("folder");
    UnitTest<Equal>(FileSystem::Classify(dirTestPath).Unwrap(), FileSystem::ObjectType::Folder);
    UnitTest<NotEqual>(FileSystem::Classify(dirTestPath).Unwrap(), FileSystem::ObjectType::File);
    UnitTest<true>(FileSystem::IsFolderEmpty(dirTestPath).Unwrap());
    UnitTest<false>(FileSystem::Remove(dirTestPath));

    FilePath tempFilePath = dirTestPath / TSTR("tempFile.txt");
    UnitTest<false>(FileSystem::CreateNewFolder(dirTestPath, {}, false));
    UnitTest<Equal>(FileSystem::IsFolderEmpty(dirTestPath).Unwrap(), true);
    Error<> fileError = DefaultError::Ok();
    UnitTest<Equal>(FileSystem::Classify(tempFilePath).GetError(), DefaultError::NotFound());
    FileToCFile(tempFilePath, FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
    UnitTest<false>(fileError);
    UnitTest<Equal>(FileSystem::IsFolderEmpty(dirTestPath).Unwrap(), false);
    UnitTest<Equal>(FileSystem::Classify(tempFilePath).Unwrap(), FileSystem::ObjectType::File);
    FilePath tempFile2Path = dirTestPath / TSTR("tempFile2.txt");
    UnitTest<false>(FileSystem::CopyTo(tempFilePath, tempFile2Path, false));
    FilePath tempFileRenamedPath = dirTestPath / TSTR("tempFileRenamed.txt");
    UnitTest<false>(FileSystem::MoveTo(tempFilePath, tempFileRenamedPath, false));
    UnitTest<Equal>(FileSystem::Classify(tempFilePath).GetError(), DefaultError::NotFound());
    UnitTest<Equal>(FileSystem::Classify(tempFileRenamedPath).Unwrap(), FileSystem::ObjectType::File);

    UnitTest<Equal>(FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), false);
    UnitTest<false>(FileSystem::IsReadOnlySet(tempFileRenamedPath, true));
    UnitTest<Equal>(FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), true);
    UnitTest<false>(FileSystem::IsReadOnlySet(tempFileRenamedPath, false));
    UnitTest<Equal>(FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), false);

    PRINTLOG("finished filesystem tests\n");
}

int main(int argc, const char **argv)
{
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

    StdLib::Initialization::PlatformAbstractionInitialize({});

    FilePath folderForTests = FilePath::FromChar(filesTestFolder);
    UnitTest<false>(FileSystem::CreateNewFolder(folderForTests, {}, true));

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
    TestFile<FileToCFile>(folderForTests);
    TestFile<File>(folderForTests);
    TestFileSharing<FileToCFile>(folderForTests);
    TestFileSharing<File>(folderForTests);
    TestFiles(folderForTests);

    UnitTest<false>(FileSystem::Remove(folderForTests));

#ifdef PLATFORM_WINDOWS
    getchar();
#endif
}