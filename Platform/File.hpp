#pragma once

#include "FilePath.hpp"
#include <IFile.hpp>

WARNING_PUSH
WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW
WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED

namespace StdLib
{
    class File : public IFile, public TypeIdentifiable<File>
    {
        friend class MemoryMappedFile;

    public:

    #ifdef STDLIB_ENABLE_FILE_STATS
        struct FileStats
        {
            using countert = ui64;
            countert writesToBufferCount;
            countert writesToFileCount;
            countert readsFromBufferCount;
            countert readsFromFileCount;
            countert bytesFromBufferRead;
            countert bytesFromFileRead;
            countert bytesToBufferWritten;
            countert bytesToFileWritten;
            countert bufferedWrites;
            countert unbufferedWrites;
            countert bufferedReads;
            countert unbufferedReads;
        };
    #endif

    private:
        fileHandle _handle = fileHandle_undefined;
    #ifdef STDLIB_ENABLE_FILE_STATS
        FileStats _stats{};
    #endif
        i64 _offsetToStart;
        bufferType _internalBuffer = {nullptr, nullptr};
        ui32 _bufferSize = 0;
        ui32 _bufferPos = 0;
        ui32 _readBufferCurrentSize = 0; // can be lower than bufferSize if, for example, EOF is reached, 0 if not reading
        bool _isOwningFileHandle = true;
        FileOpenMode _openMode;
        FileProcModes::FileProcMode _procMode = FileProcModes::Read;
        FileCacheModes::FileCacheMode _cacheMode;

    #ifdef PLATFORM_WINDOWS
        FilePath _pnn; // used only on WindowsXP where you can't get PNN from the file handle
    #endif

    public:
        ~File();
        File() = default;
        File(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None, Error<> *error = nullptr);
        File(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset = 0, Error<> *error = nullptr);

        File(File &&other) noexcept;
        File &operator = (File &&other) noexcept;

        [[nodiscard]] Error<> Open(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None);
        [[nodiscard]] Error<> Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset = 0);

    #ifdef STDLIB_ENABLE_FILE_STATS
		[[must_be_open]] [[nodiscard]] FileStats StatsGet() const;
		[[must_be_open]] void StatsReset();
    #endif

		[[must_be_open]] [[nodiscard]] FileOpenMode OpenMode() const;

		[[must_be_open]] [[nodiscard]] Result<FilePath> PNN() const;

		[[must_be_open]] [[nodiscard]] fileHandle OsFileDescriptor() const;

		[[must_be_open]] [[nodiscard]] fileHandle CloseAndGetOsFileDescriptor();

		[[must_be_open]] bool FlushInternal(); // flushes only internal buffers, doesn't flush system caches

		[[nodiscard]] virtual TypeId Type() const override;

        virtual void Close() override;
        [[nodiscard]] virtual bool IsOpen() const override;

		[[must_be_open]] virtual bool Read(void *RSTR target, ui32 len, ui32 *RSTR read = 0) override;
		[[must_be_open]] virtual bool Write(const void *source, ui32 len, ui32 *RSTR written = 0) override;

		[[must_be_open]] virtual bool Flush() override;

        [[nodiscard]] virtual bool IsBufferingSupported() const override;
		[[must_be_open]] virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) override;
		[[must_be_open]] [[nodiscard]] virtual std::pair<ui32, const void *> BufferGet() const override;

        [[nodiscard]] virtual bool IsSeekSupported() const override;

		[[must_be_open]] [[nodiscard]] virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) override;
		[[must_be_open]] virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) override;

		[[must_be_open]] [[nodiscard]] virtual Result<ui64> SizeGet() override;
		[[must_be_open]] virtual Error<> SizeSet(ui64 newSize) override;

		[[must_be_open]] [[nodiscard]] virtual FileProcModes::FileProcMode ProcMode() const override;
		[[must_be_open]] [[nodiscard]] virtual FileCacheModes::FileCacheMode CacheMode() const override;

    private:
		bool PerformFlush(bool isFlushSystemCaches);
        void FlushSystemCaches();
		[[nodiscard]] bool WriteToFile(const void *source, ui32 len, ui32 *written);
		[[nodiscard]] bool ReadFromFile(void *target, ui32 len, ui32 *read);
		[[nodiscard]] bool CancelCachedRead();
		[[nodiscard]] Result<i64> CurrentFileOffset() const;
    };
}

WARNING_POP

#ifdef STDLIB_ENABLE_FILE_STATS
	#pragma detect_mismatch("STDLIB_ENABLE_FILE_STATS", "1")
#else
	#pragma detect_mismatch("STDLIB_ENABLE_FILE_STATS", "0")
#endif