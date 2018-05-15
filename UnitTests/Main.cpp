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

static void ErrorTests()
{
    auto okError = DefaultError::Ok();
    ASSUME(okError.IsOk());
    ASSUME(!okError);
    ASSUME(okError == DefaultError::Ok());
    ASSUME(okError != DefaultError::UnknownError());

    auto unknownFormatWithAttachment = Error<std::string>(DefaultError::UnknownFormat(), "MPEG");
    ASSUME(!unknownFormatWithAttachment.IsOk());
    ASSUME(unknownFormatWithAttachment);
    ASSUME(unknownFormatWithAttachment == DefaultError::UnknownFormat());
    ASSUME(unknownFormatWithAttachment.Attachment() == "MPEG");

    auto busyWithCustomDescription = Error<void, std::string>(DefaultError::Busy(), "Bus was handling "s + std::to_string(284) + " other requrests"s);
    ASSUME(!busyWithCustomDescription.IsOk());
    ASSUME(busyWithCustomDescription);
    ASSUME(busyWithCustomDescription == DefaultError::Busy());
    ASSUME(busyWithCustomDescription.Description() == "Bus was handling "s + std::to_string(284) + " other requrests"s);

    struct Canceller {};
    auto canceller = std::make_shared<Canceller>();
    auto cancelledWithAttachmentAndCustomDescription = Error<std::shared_ptr<Canceller>, std::string>(DefaultError::Cancelled(), canceller, "Cancelling reason: "s + "interrupted by user"s);
    ASSUME(!cancelledWithAttachmentAndCustomDescription.IsOk());
    ASSUME(cancelledWithAttachmentAndCustomDescription);
    ASSUME(cancelledWithAttachmentAndCustomDescription == DefaultError::Cancelled());
    ASSUME(AreSharedPointersEqual(cancelledWithAttachmentAndCustomDescription.Attachment(), canceller));
    ASSUME(cancelledWithAttachmentAndCustomDescription.Description() == "Cancelling reason: "s + "interrupted by user"s);

    cancelledWithAttachmentAndCustomDescription = decltype(cancelledWithAttachmentAndCustomDescription)::FromDefaultError(DefaultError::Ok());
    ASSUME(cancelledWithAttachmentAndCustomDescription.IsOk());
    ASSUME(!cancelledWithAttachmentAndCustomDescription);
    ASSUME(cancelledWithAttachmentAndCustomDescription == DefaultError::Ok());
    ASSUME(cancelledWithAttachmentAndCustomDescription.Attachment() == std::shared_ptr<Canceller>());
    ASSUME(cancelledWithAttachmentAndCustomDescription.Description() == "");

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
    ASSUME(!customErrorWithAttachmentAndCustomDescription.IsOk());
    ASSUME(customErrorWithAttachmentAndCustomDescription);
    ASSUME(customErrorWithAttachmentAndCustomDescription == createCustomErrorUnitTest());
    ASSUME(customErrorWithAttachmentAndCustomDescription.Attachment() == -45);
    ASSUME(customErrorWithAttachmentAndCustomDescription.Description() == "UnitTesting "s + std::to_string(-45));

    PRINTLOG("finished error tests\n");
}

static void ResultTests()
{
    Result<i32> intResult(43);
    ASSUME(intResult.IsOk());
    ASSUME(intResult);
    ASSUME(intResult.GetError() == DefaultError::Ok());
    ASSUME(intResult.Unwrap() == 43);

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
            ASSUME(isExcepted == IsExpectingException); \
        }
#else
    #define EXCEPTION_CHECK(Code, IsExpectingException)
#endif

static void VirtualMemoryTests()
{
    void *memory = VirtualMemory::Reserve(999);
    ASSUME(memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    auto commitError = VirtualMemory::Commit(memory, 500, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    ASSUME(commitError.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    auto protection = VirtualMemory::PageModeGet(memory, VirtualMemory::PageSize());
#ifdef PLATFORM_WINDOWS
    ASSUME(protection.Unwrap() == (VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write));
#else
    ASSUME(protection.GetError() == DefaultError::Unsupported());
#endif

    auto protectionSetResult = VirtualMemory::PageModeSet(memory, VirtualMemory::PageSize(), VirtualMemory::PageMode::Read);
    ASSUME(protectionSetResult.IsOk());
    EXCEPTION_CHECK(memset(memory, 0, 10), true);

    ASSUME(VirtualMemory::Free(memory, 999));

    memory = VirtualMemory::Alloc(999, VirtualMemory::PageMode::Read + VirtualMemory::PageMode::Write);
    ASSUME(memory);
    EXCEPTION_CHECK(memset(memory, 0, 10), false);

    ASSUME(VirtualMemory::Free(memory, 999));

    PRINTLOG("finished virtual memory tests\n");
}

static void AllocatorsTests()
{
    void *memory = Allocator::MallocBased::Allocate(100);
    ASSUME(memory);
    uiw blockSize = Allocator::MallocBased::MemorySize(memory);
    ASSUME(blockSize >= 100);
    bool expandResult = Allocator::MallocBased::ReallocateInplace(memory, 200);
    if (expandResult)
    {
        blockSize = Allocator::MallocBased::MemorySize(memory);
        ASSUME(blockSize >= 200);
    }
    memory = Allocator::MallocBased::Reallocate(memory, 300);
    ASSUME(memory);
    blockSize = Allocator::MallocBased::MemorySize(memory);
    ASSUME(blockSize >= 300);

    Allocator::MallocBased::Free(memory);

    PRINTLOG("finished allocators tests\n");
}

void FilePathTests()
{
    FilePath path{L"C:\\ext\\\\no.txt"};

    path.Normalize();
    ASSUME(path.PlatformPath() == L"C:/ext/no.txt");
    auto fileName = path.FileNameView();
    ASSUME(fileName == L"no");
    auto fileExt = path.ExtensionView();
    ASSUME(fileExt == L"txt");
    path.ReplaceExtension(L"bmp");
    fileExt = path.ExtensionView();
    ASSUME(fileExt == L"bmp");
    path.ReplaceFileName(L"yes");
    fileName = path.FileNameView();
    ASSUME(fileName == L"yes");
    ASSUME(path.PlatformPath() == L"C:/ext/yes.bmp");
    ASSUME(path.FileNameExtView() == L"yes.bmp");
    path.RemoveLevel();
    ASSUME(path.PlatformPath() == L"C:/ext/");
    path.RemoveLevel();
    ASSUME(path.PlatformPath() == L"C:/");
    path += L"folder";
    ASSUME(path.PlatformPath() == L"C:/folder");
    ASSUME(path.FileNameView() == L"folder");
    ASSUME(path.FileNameExtView() == L"folder");
    ASSUME(path.HasFileName());
    ASSUME(!path.ExtensionView());
    ASSUME(!path.HasExtension());
    path /= L"file.ext";
    ASSUME(path.PlatformPath() == L"C:/folder/file.ext");
    path.ReplaceFileNameExt(L"new.fil");
    ASSUME(path.PlatformPath() == L"C:/folder/new.fil");

    PRINTLOG("finished FilePath tests\n");
}

static void FileWrite(IFile &file)
{
    ASSUME((file.ProcMode() + FileProcMode::Write) == file.ProcMode());
    ASSUME(file.OffsetGet().Unwrap() == 0);
    
    std::string_view str = "test start";
    ui32 written = 0;
    ASSUME(file.Write(str.data(), str.length(), &written));
    ASSUME(written == str.length());
    ASSUME(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap() == str.length());
    ASSUME(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap() == 0);
    ASSUME(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap() == 0);
    
    if (file.IsSeekSupported())
    {
        i64 oldOffset = file.OffsetGet().Unwrap();
        ASSUME(file.OffsetSet(FileOffsetMode::FromCurrent, -5));
        ASSUME(file.OffsetGet(FileOffsetMode::FromBegin).Unwrap() == 5);
        ASSUME(file.OffsetGet(FileOffsetMode::FromCurrent).Unwrap() == 0);
        ASSUME(file.OffsetGet(FileOffsetMode::FromEnd).Unwrap() == -5);
        str = "trats";
        ASSUME(file.Write(str.data(), str.length(), &written));
        ASSUME(written == str.length());
        ASSUME(file.OffsetGet().Unwrap() == oldOffset);
    }

    auto startSize = "test start"s.length();
    ASSUME(file.SizeGet().Unwrap() == startSize);
    ASSUME(!file.SizeSet(startSize - 1));
    ASSUME(file.SizeGet().Unwrap() == startSize - 1);
    ASSUME(!file.SizeSet(startSize + 1));
    ASSUME(file.SizeGet().Unwrap() == startSize + 1);
    ASSUME(!file.SizeSet(startSize - 1));
    ASSUME(file.SizeGet().Unwrap() == startSize - 1);
}

static void FileRead(IFile &file)
{
    ASSUME(file.ProcMode() == FileProcMode::Read);
    ASSUME(file.OffsetGet().Unwrap() == 0);

    std::string_view str = file.IsSeekSupported() ? "test trat" : "test star";
    ASSUME(file.SizeGet().Unwrap() == str.length());
    ui32 read = 0;
    std::string target(str.length(), '\0');
    ASSUME(file.Read(target.data(), target.length(), &read));
    ASSUME(read == str.length());
    ASSUME(str == target);
    ASSUME(file.OffsetGet().Unwrap() == str.length());

    if (file.IsSeekSupported())
    {
        ASSUME(file.OffsetSet(FileOffsetMode::FromCurrent, -4));
        str = file.IsSeekSupported() ? "trat" : "star";
        target.resize(str.length());
        ASSUME(file.Read(target.data(), target.length(), &read));
        ASSUME(read == str.length());
        ASSUME(target == str);
    }
}

static void FileAppendWrite(IFile &file)
{
    ASSUME((file.ProcMode() + FileProcMode::WriteAppend) == file.ProcMode());
    ASSUME(file.OffsetGet().Unwrap() == 0);
    std::string_view str = "9184";
    ui32 written = 0;
    ASSUME(file.Write(str.data(), str.length(), &written));
    ASSUME(written == str.length());
    ASSUME(file.OffsetGet().Unwrap() == str.length());
    ASSUME(file.SizeGet().Unwrap() == str.length());
}

static void FileAppendRead(IFile &file)
{

    ASSUME(file.ProcMode() == FileProcMode::Read);
    ASSUME(file.OffsetGet().Unwrap() == 0);
    std::string_view str = file.IsSeekSupported() ? "test trat9184" : "test star9184";
    ASSUME(file.SizeGet().Unwrap() == str.length());
    std::string target(str.length(), '\0');
    ui32 read = 0;
    ASSUME(file.Read(target.data(), target.length(), &read));
    ASSUME(read == str.length());
    ASSUME(target == str);
}

static void TestFileToMemoryStream()
{
    MemoryStreamAllocator<> memoryStream;
    
    Error<> fileError = DefaultError::Ok();
    FileToMemoryStream file = FileToMemoryStream(memoryStream, FileProcMode::Write, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileRead(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::WriteAppend, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileAppendWrite(file);

    file = FileToMemoryStream(memoryStream, FileProcMode::Read, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileAppendRead(file);

    PRINTLOG("finished file memory stream tests\n");
}

static void TestFileToCFile(const FilePath &folderForTests)
{
    Error<> fileError = DefaultError::Ok();
    FileToCFile file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::CreateAlways, FileProcMode::Write, FileCacheMode::Default, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileWrite(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileRead(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::WriteAppend, FileCacheMode::Default, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileAppendWrite(file);

    file = FileToCFile(folderForTests / TSTR("fileToCFILE.txt"), FileOpenMode::OpenExisting, FileProcMode::Read, FileCacheMode::Default, &fileError);
    ASSUME(!fileError && file.IsOpened());
    FileAppendRead(file);

    PRINTLOG("finished file to C FILE tests\n");
}

int main()
{
    StdLib::Initialization::PlatformAbstractionInitialize({});

    FilePath folderForTests = TSTR("FileTestsFolder");
    ASSUME(!Files::CreateNewFolder(folderForTests, {}, true));

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