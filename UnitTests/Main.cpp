#include <StdPlatformAbstractionLib.hpp>
#include <FileToMemoryStream.hpp>
#include <FileToCFile.hpp>
#include <FileSystem.hpp>
#include <File.hpp>
#include <MemoryMappedFile.hpp>
#include <TimeMoment.hpp>
#include <thread>
#include <MathFunctions.hpp>

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
    LeftLesser,
    LeftLesserEqual,
    Equal,
    LeftGreaterEqual,
    LeftGreater,
    NotEqual
};

template <typename T> void Assume(T &&condition, const char *condStr, const char *argsStr, int line, const char *file)
{
    if (!(condition))
    {
        printf("condition %s failed for arguments %s, file %s line %i\n", condStr, argsStr, file, line);
        HARDBREAK;
    }
}

template <UnitTestType testType, typename T, typename E> void UnitTest(T &&left, E &&right, const char *condStr, const char *argsStr, int line, const char *file)
{
    if constexpr (testType == LeftLesser) Assume(left < right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftLesserEqual) Assume(left <= right, condStr, argsStr, line, file);
    else if constexpr (testType == Equal) Assume(left == right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftGreaterEqual) Assume(left >= right, condStr, argsStr, line, file);
    else if constexpr (testType == LeftGreater) Assume(left > right, condStr, argsStr, line, file);
    else if constexpr (testType == NotEqual) Assume(left != right, condStr, argsStr, line, file);
}

template <bool isTrue, typename T> void UnitTest(T &&value, const char *condStr, const char *argsStr, int line, const char *file)
{
    if constexpr (isTrue)
    {
        Assume(value, condStr, argsStr, line, file);
    }
    else
    {
        Assume(!value, condStr, argsStr, line, file);
    }
}

#define UTest(cond, ...) UnitTest<cond>(__VA_ARGS__, TOSTR(cond), /*TOSTR(__VA_ARGS__)**/"", __LINE__, __FILE__)

static void SetBitTests()
{
    ui32 value = 0;
    value = SetBit(value, 14u, true);
    UTest(Equal, value, 16384u);
    UTest(true, IsBitSet(value, 14u));
    UTest(false, IsBitSet(value, 13u));
    value = SetBit(value, 14u, false);
    UTest(Equal, value, 0u);

    PRINTLOG("finished set bit tests\n");
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

    PRINTLOG("finished significant bit tests\n");
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

    PRINTLOG("finished change endianness tests\n");
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

    PRINTLOG("finished rotate bits tests\n");
}

static void ErrorTests()
{
    auto okError = DefaultError::Ok();
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

    PRINTLOG("finished error tests\n");
}

static void ResultTests()
{
    Result<i32> intResult(43);
    UTest(true, intResult.IsOk());
    UTest(true, intResult);
    UTest(Equal, intResult.GetError(), DefaultError::Ok());
    UTest(Equal, intResult.Unwrap(), 43);

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
            UTest(Equal, isExcepted, IsExpectingException); \
        }
#else
    #define EXCEPTION_CHECK(Code, IsExpectingException)
#endif

static void VirtualMemoryTests()
{
    void *memory = VirtualMemory::Reserve(999);
    UTest(true, memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    auto commitError = VirtualMemory::Commit(memory, 500, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    UTest(true, commitError.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    auto protection = VirtualMemory::PageModeGet(memory, VirtualMemory::PageSize());
#ifdef PLATFORM_WINDOWS
    UTest(Equal, protection.Unwrap(), (VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write));
#else
    UTest(Equal, protection.GetError(), DefaultError::Unsupported());
#endif

    auto protectionSetResult = VirtualMemory::PageModeSet(memory, VirtualMemory::PageSize(), VirtualMemory::PageMode::Read);
    UTest(true, protectionSetResult.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    UTest(true, VirtualMemory::Free(memory, 999));

    memory = VirtualMemory::Alloc(999, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    UTest(true, memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    UTest(true, VirtualMemory::Free(memory, 999));

    PRINTLOG("finished virtual memory tests\n");
}

static void AllocatorsTests()
{
    uiw blockSize = 0;

    void *memory = Allocator::MallocBased::Allocate(100);
    UTest(true, memory);

#ifndef PLATFORM_ANDROID
    blockSize = Allocator::MallocBased::MemorySize(memory);
    UTest(LeftGreaterEqual, blockSize, 100);
#endif

    bool expandResult = Allocator::MallocBased::ReallocateInplace(memory, 200);
    if (expandResult)
    {
        blockSize = Allocator::MallocBased::MemorySize(memory);
        UTest(LeftGreaterEqual, blockSize, 200);
    }
    memory = Allocator::MallocBased::Reallocate(memory, 300);
    UTest(true, memory);

#ifndef PLATFORM_ANDROID
    blockSize = Allocator::MallocBased::MemorySize(memory);
    UTest(LeftGreaterEqual, blockSize, 300);
#endif

    Allocator::MallocBased::Free(memory);

    PRINTLOG("finished allocators tests\n");
}

void FilePathTests()
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

    PRINTLOG("finished FilePath tests\n");
}

static void FileWrite(IFile &file)
{
    UTest(Equal, (file.ProcMode() + FileProcMode::Write), file.ProcMode());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), 0);
    
    std::string_view str = "test0123456789 start";
    ui32 written = 0;
    UTest(true, file.Write(str.data(), (ui32)str.length(), &written));
    UTest(Equal, written, str.length());
    UTest(true, file.Write(nullptr, 0, &written));
    UTest(Equal, written, 0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), str.length());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), 0);
    
    if (file.IsSeekSupported())
    {
        i64 oldOffset = file.OffsetGet().Unwrap();
        UTest(true, file.OffsetSet(FileOffsetMode::FromCurrent, -5));
        UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), str.length() - 5);
        UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), 0);
        UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -5);
        str = "trats";
        UTest(true, file.Write(str.data(), (ui32)str.length(), &written));
        UTest(Equal, written, str.length());
        UTest(Equal, file.OffsetGet().Unwrap(), oldOffset);
    }

    auto startSize = "test0123456789 start"s.length();
    UTest(Equal, file.SizeGet().Unwrap(), startSize);
    UTest(false, file.SizeSet(startSize - 1));
    UTest(Equal, file.SizeGet().Unwrap(), startSize - 1);
    UTest(false, file.SizeSet(startSize + 1));
    UTest(Equal, file.SizeGet().Unwrap(), startSize + 1);
    UTest(false, file.SizeSet(startSize - 1));
    UTest(Equal, file.SizeGet().Unwrap(), startSize - 1);
}

static void FileRead(IFile &file)
{
    UTest(Equal, file.ProcMode(), FileProcMode::Read);
    UTest(Equal, file.OffsetGet().Unwrap(), 0);

    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat" : "test0123456789 star";
    UTest(LeftGreaterEqual, file.SizeGet().Unwrap(), str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    UTest(true, file.Read(target.data(), (ui32)target.length(), &read));
    UTest(Equal, read, str.length());
    UTest(Equal, str, target);
    UTest(Equal, file.OffsetGet().Unwrap(), str.length());
    UTest(true, file.Read(nullptr, 0, &read));
    UTest(Equal, read, 0);
    UTest(Equal, file.OffsetGet().Unwrap(), str.length());

    if (file.IsSeekSupported())
    {
        UTest(true, file.OffsetSet(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        UTest(true, file.Read(target.data(), (ui32)target.length(), &read));
        UTest(Equal, read, str.length());
        UTest(Equal, target, str);
    }
}

static void FileAppendWrite(IFile &file)
{
    UTest(Equal, (file.ProcMode() + FileProcMode::WriteAppend), file.ProcMode());
    UTest(Equal, file.OffsetGet().Unwrap(), 0);
    std::string_view str = "9184";
    ui32 written = 0;
    UTest(true, file.Write(str.data(), (ui32)str.length(), &written));
    UTest(Equal, written, str.length());
    UTest(Equal, file.OffsetGet().Unwrap(), str.length());
    UTest(Equal, file.SizeGet().Unwrap(), str.length());
}

static void FileAppendRead(IFile &file)
{
    UTest(Equal, file.ProcMode(), FileProcMode::Read);
    UTest(Equal, file.OffsetGet().Unwrap(), 0);
    std::string_view str = file.IsSeekSupported() ? "test0123456789 trat9184" : "test0123456789 star9184";
    UTest(Equal, file.SizeGet().Unwrap(), str.length());
    std::string target(str.length(), '\0');
    ui32 read = 0;
    UTest(true, file.Read(target.data(), (ui32)target.length(), &read));
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

    ui32 written, read;

    char readBuf[256];

    UTest(Equal, (file.ProcMode() + FileProcMode::Write + FileProcMode::Read), file.ProcMode());

    UTest(Equal, file.OffsetGet().Unwrap(), 0);
    UTest(true, file.Write(crapString0.data(), (ui32)crapString0.length(), &written));
    UTest(Equal, written, (ui32)crapString0.length());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)crapString0.length());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), (i64)0);
    UTest(Equal, file.SizeGet().Unwrap(), crapString0.length());

    UTest(true, file.OffsetSet(FileOffsetMode::FromBegin, (i64)32));
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)32);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -((i64)crapString0.length() - 32));
    UTest(true, file.Read(readBuf, (ui32)crapString0.length() - 32, &read));
    UTest(Equal, read, (ui32)crapString0.length() - 32);
    UTest(true, !memcmp(readBuf, crapString0.data() + 32, crapString0.length() - 32));
    UTest(Equal, file.SizeGet().Unwrap(), crapString0.length());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)crapString0.length());
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), (i64)0);

    UTest(true, file.OffsetSet(FileOffsetMode::FromBegin, 64));
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromBegin).Unwrap(), (i64)64);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap(), (i64)0);
    UTest(Equal, file.OffsetGet(FileOffsetMode::FromEnd).Unwrap(), -(i64)(crapString0.length() - 64));
    UTest(Equal, file.SizeGet().Unwrap(), crapString0.length());
    UTest(true, file.Write(crapString1.data(), (ui32)crapString1.length(), &written));
    UTest(Equal, written, (ui32)crapString1.length());

    UTest(true, file.OffsetSet(FileOffsetMode::FromBegin, 0));
    UTest(Equal, file.SizeGet().Unwrap(), crapString0.length());
    crapString0.replace(crapString0.begin() + 64, crapString0.begin() + 64 + crapString1.length(), crapString1.data(), crapString1.length());
    UTest(true, file.Read(readBuf, (ui32)crapString0.length() + 999, &read)); // read outside the file, must truncate the requested size
    UTest(Equal, read, (ui32)crapString0.length());
    UTest(true, !memcmp(readBuf, crapString0.data(), crapString0.length()));
}

static void TestFileToMemoryStream()
{
    MemoryStreamAllocator<> memoryStream;
    
    Error<> fileError = DefaultError::Ok();
    FileToMemoryStream file = FileToMemoryStream(memoryStream, FileProcMode::Write, &fileError);
    UTest(true, !fileError && file.IsOpened());
    FileWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    UTest(true, !fileError && file.IsOpened());
    FileRead(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::WriteAppend, &fileError);
    UTest(true, !fileError && file.IsOpened());
    FileAppendWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    UTest(true, !fileError && file.IsOpened());
    FileAppendRead(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read + FileProcMode::Write, &fileError);
    UTest(true, !fileError && file.IsOpened());
    FileWriteRead(file);

    PRINTLOG("finished file memory stream tests\n");
}

template <typename T> void TestFile(const FilePath &folderForTests)
{
    auto internalTest = [folderForTests](ui32 bufferSize, File::bufferType &&buffer)
    {
        Error<> fileError = DefaultError::Ok();
        T file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
        UTest(true, !fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileWrite(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
        UTest(true, !fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileRead(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::WriteAppend, FileCacheMode::Default, FileShareMode::None, &fileError);
        UTest(true, !fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileAppendWrite(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
        UTest(true, !fileError && file.IsOpened());
        file.BufferSet(bufferSize, move(buffer));
        FileAppendRead(file);
        file.Close();

        file = T(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Read + FileProcMode::Write, FileCacheMode::Default, FileShareMode::Read + FileShareMode::Write, &fileError);
        UTest(true, !fileError && file.IsOpened());
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
    Error<> fileError = DefaultError::Ok();
    T file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
    UTest(true, !fileError && file.IsOpened());

    T file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::None, &fileError);
    UTest(true, fileError && !file2.IsOpened());

    file.Close();
    file2 = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Read, &fileError);
    UTest(true, !fileError && file2.IsOpened());

    file2.Close();
    file = T(folderForTests / TSTR("fileSharingTest.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, FileShareMode::Write, &fileError);
    UTest(true, !fileError && file.IsOpened());

    PRINTLOG("finished template file sharing tests\n");
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
    FileToCFile(tempFilePath, FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, FileShareMode::None, &fileError);
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

    UTest(Equal, FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), false);
    UTest(false, FileSystem::IsReadOnlySet(tempFileRenamedPath, true));
    UTest(Equal, FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), true);
    UTest(false, FileSystem::IsReadOnlySet(tempFileRenamedPath, false));
    UTest(Equal, FileSystem::IsReadOnlyGet(tempFileRenamedPath).Unwrap(), false);

    PRINTLOG("finished filesystem tests\n");
}

static void TestMemoryMappedFile(const FilePath &folderForTests)
{
    std::string crapString(256, '\0');
    for (char &ch : crapString)
    {
        ch = 32 + rand() % 96;
    }

    char tempBuf[256];

    File file = File(folderForTests / TSTR("memMapped.txt"), FileOpenMode::CreateAlways, FileProcMode::Read + FileProcMode::Write);
    UTest(true, file.IsOpened());
    UTest(true, file.Write(crapString.data(), (ui32)crapString.length()));

    Error<> error = DefaultError::Ok();
    MemoryMappedFile mapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mapping.IsOpened());
    UTest(Equal, mapping.Size(), crapString.length());
    UTest(Equal, mapping.IsWritable(), true);
    UTest(true, !memcmp(crapString.data(), mapping.Memory(), crapString.size()));

    MemoryMappedFile mappingDuplicate = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mappingDuplicate.IsOpened());
    UTest(NotEqual, mapping.Memory(), mappingDuplicate.Memory());

    MemoryMappedFile mappingOffsetted = MemoryMappedFile(file, 2, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mappingOffsetted.IsOpened());

    std::reverse(crapString.begin(), crapString.end());
    UTest(false, !memcmp(crapString.data(), mapping.Memory(), crapString.size()));

    UTest(true, file.OffsetSet(FileOffsetMode::FromBegin, 0));
    UTest(true, file.Write(crapString.data(), (ui32)crapString.length()));
    UTest(true, file.Flush());

    UTest(true, !memcmp(crapString.data(), mapping.Memory(), crapString.size()));
    std::reverse(crapString.begin(), crapString.end());
    UTest(false, !memcmp(crapString.data(), mapping.Memory(), crapString.size()));

    memcpy(mapping.Memory(), crapString.data(), crapString.length());
    mapping.Flush();
    UTest(true, file.OffsetSet(FileOffsetMode::FromBegin, 0));
    UTest(true, file.Read(tempBuf, (ui32)crapString.length()));
    UTest(true, !memcmp(crapString.data(), tempBuf, crapString.length()));

    file.Close();
    UTest(true, !memcmp(crapString.data(), mapping.Memory(), crapString.size()));

    UTest(true, !memcmp(mapping.Memory(), mappingDuplicate.Memory(), crapString.size())); // different mapping should be coherent

    UTest(true, !memcmp(mapping.Memory() + 2, mappingOffsetted.Memory(), crapString.size() - 2));

    file = File(folderForTests / TSTR("memMapped2.txt"), FileOpenMode::CreateAlways, FileProcMode::Read + FileProcMode::Write);
    UTest(true, file.IsOpened());
    FileWrite(file);
    mapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, mapping.IsOpened());
    auto memoryStream = mapping.ToMemoryStream();
    FileToMemoryStream memoryStreamFile = FileToMemoryStream(memoryStream, FileProcMode::Read);
    UTest(true, memoryStreamFile.IsOpened());
    FileRead(memoryStreamFile);

    file = File(folderForTests / TSTR("appendTest.txt"), FileOpenMode::CreateAlways, FileProcMode::Write);
    UTest(true, file.IsOpened());
    std::string_view invisiblePartStr = "invisible part";
    UTest(true, file.Write(invisiblePartStr.data(), (ui32)invisiblePartStr.length()));
    file.Close();
    file = File(folderForTests / TSTR("appendTest.txt"), FileOpenMode::OpenExisting, FileProcMode::WriteAppend + FileProcMode::Read);
    std::string_view currentPartStr = "current part";
    UTest(true, file.Write(currentPartStr.data(), (ui32)currentPartStr.length()));

    MemoryMappedFile appendedFileMapping = MemoryMappedFile(file, 0, uiw_max, false, false, &error);
    UTest(false, error);
    UTest(true, appendedFileMapping.IsOpened());
    UTest(true, !memcmp(appendedFileMapping.Memory(), currentPartStr.data(), currentPartStr.size()));

    PRINTLOG("finished memory mapped file tests\n");
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
    UTest(LeftGreater, moment + 0.5f, moment2);
    UTest(LeftGreater, moment, moment2 - 0.5f);
    TimeDifference diff = moment2 - moment;
    f32 diffSeconds = diff.ToSeconds();
    UTest(LeftGreater, diffSeconds, 0.0001f);
    UTest(LeftGreaterEqual, TimeMoment::Now(), moment2);
    UTest(LeftGreaterEqual, TimeMoment::Now(), moment);
    UTest(true, EqualsWithEpsilon(((moment + diff) - moment2).ToSeconds(), 0.0f, 0.0001f));
    UTest(true, EqualsWithEpsilon(((moment2 - diff) - moment).ToSeconds(), 0.0f, 0.0001f));
    TimeDifference refDiff = 0.5f;
    UTest(true, EqualsWithEpsilon(refDiff.ToSeconds(), 0.5f, 0.0001f));
    UTest(LeftLesser, TimeDifference(0.2f), 0.5f);

    PRINTLOG("finished time moment tests\n");
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
        NonCopyable(NonCopyable &&)
        {
            ++_moveCalledTimes;
        }
        NonCopyable &operator = (NonCopyable &&)
        {
            ++_moveCalledTimes;
            return *this;
        }
    };

    struct NonCopyable2 : NonCopyable
    {
        ui8 _crap[56];
    };

    struct NonCopyable3 : NonCopyable2
    {
        i32 _extra0, _extra1;
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
    //holder.Destroy<NonCopyable>();

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
    //holder3.Destroy<NonCopyable>();

    PRINTLOG("finished data holder tests\n");
}

static void MemoryStreamTests()
{
    std::string_view test0 = "Luna";
    std::string_view test1 = "Celestia";

    auto checkContent = [test0, test1](IMemoryStream &ms)
    {
        UTest(true, ms.IsReadable());
        UTest(Equal, ms.Size(), test0.length() + test1.length());
        UTest(true, !memcmp(ms.CMemory(), test0.data(), test0.length()));
        UTest(true, !memcmp(ms.CMemory() + test0.length(), test1.data(), test1.length()));
    };

    auto writeAndCheck = [test0, test1, checkContent](IMemoryStream &ms)
    {
        UTest(true, ms.IsReadable());
        UTest(Equal, ms.Resize(test0.length()), test0.length());
        memcpy(ms.Memory(), test0.data(), test0.length());
        UTest(Equal, ms.Resize(ms.Size() + test1.size()), test0.length() + test1.length());
        memcpy(ms.Memory() + test0.length(), test1.data(), test1.size());
        checkContent(ms);
    };

    MemoryStreamFixed<128> fixedTest;
    writeAndCheck(fixedTest);

    MemoryStreamAllocator<> allocatorTest;
    writeAndCheck(allocatorTest);

    char buf[128];
    MemoryStreamFixedExternal externalTest = MemoryStreamFixedExternal(buf, Funcs::CountOf(buf));
    writeAndCheck(externalTest);

    struct HolderTestData
    {
        char str[13] = "LunaCelestia";
    } holderTestData;

    using holderType = DataHolder<32>;
    holderType data = holderType(holderTestData);

    auto holderTestDataProvide = [](const holderType &data) -> const ui8 *
    {
        return (ui8 *)&data.Get<HolderTestData>().str;
    };

    auto dataHolderMS = MemoryStreamFromDataHolder<32>::New<HolderTestData>(std::move(data), test0.length() + test1.length(), holderTestDataProvide);
    checkContent(dataHolderMS);

    PRINTLOG("finished memory stream tests\n");
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
    UTest(false, FileSystem::CreateNewFolder(folderForTests, {}, true));

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
    //TestFileSharing<FileToCFile>(folderForTests);
    //TestFileSharing<File>(folderForTests);
    TestFileSystem(folderForTests);
    TestMemoryMappedFile(folderForTests);
    TimeMomentTests();
    DataHolderTests();
    MemoryStreamTests();

    Error<> folderForTestsRemove = FileSystem::Remove(folderForTests);
    printf("%s\n", folderForTestsRemove.Description());
    UTest(false, folderForTestsRemove);

#ifdef PLATFORM_WINDOWS
    getchar();
#endif
}