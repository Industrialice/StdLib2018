#include <StdPlatformAbstractionLib.hpp>
#include <FileToMemoryStream.hpp>
#include <FileToCFile.hpp>
#include <Files.hpp>

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
    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);
    
    std::string_view str = "test start";
    ui32 written = 0;
    UnitTest<true>(file.Write(str.data(), str.length(), &written));
    UnitTest<Equal>(written, str.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), str.length());
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
    UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), 0);
    
    if (file.IsSeekSupported())
    {
        i64 oldOffset = file.OffsetGet().Unwrap();
        UnitTest<true>(file.OffsetSet(FileOffsetMode::FromCurrent, -5));
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), 5);
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
        UnitTest<Equal>(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -5);
        str = "trats";
        UnitTest<true>(file.Write(str.data(), str.length(), &written));
        UnitTest<Equal>(written, str.length());
        UnitTest<Equal>(file.OffsetGet().Unwrap(), oldOffset);
    }

    auto startSize = "test start"s.length();
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

    std::string_view str = file.IsSeekSupported() ? "test trat" : "test star";
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    UnitTest<true>(file.Read(target.data(), target.length(), &read));
    UnitTest<Equal>(read, str.length());
    UnitTest<Equal>(str, target);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), str.length());

    if (file.IsSeekSupported())
    {
        UnitTest<true>(file.OffsetSet(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        UnitTest<true>(file.Read(target.data(), target.length(), &read));
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
    UnitTest<true>(file.Write(str.data(), str.length(), &written));
    UnitTest<Equal>(written, str.length());
    UnitTest<Equal>(file.OffsetGet().Unwrap(), str.length());
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
}

static void FileAppendRead(IFile &file)
{

    UnitTest<Equal>(file.ProcMode(), FileProcMode::Read);
    UnitTest<Equal>(file.OffsetGet().Unwrap(), 0);
    std::string_view str = file.IsSeekSupported() ? "test trat9184" : "test star9184";
    UnitTest<Equal>(file.SizeGet().Unwrap(), str.length());
    std::string target(str.length(), '\0');
    ui32 read = 0;
    UnitTest<true>(file.Read(target.data(), target.length(), &read));
    UnitTest<Equal>(read, str.length());
    UnitTest<Equal>(target, str);
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

    PRINTLOG("finished file memory stream tests\n");
}

static void TestFileToCFile(const FilePath &folderForTests)
{
    Error<> fileError = DefaultError::Ok();
    FileToCFile file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileWrite(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileRead(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::WriteAppend, FileCacheMode::Default, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileAppendWrite(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, &fileError);
    UnitTest<true>(!fileError && file.IsOpened());
    FileAppendRead(file);

    PRINTLOG("finished file to C FILE tests\n");
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
    UnitTest<false>(Files::CreateNewFolder(folderForTests, {}, true));

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
    TestFileToCFile(folderForTests);

#ifdef PLATFORM_WINDOWS
    getchar();
#endif
}