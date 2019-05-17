#pragma once

#include "FilePath.hpp"
#include <IFile.hpp>

namespace StdLib
{
#define MUST_BE_OPEN

    class File : public IFile
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

        File(File &&other);
        File &operator = (File &&other);

        [[nodiscard]] Error<> Open(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None);
        [[nodiscard]] Error<> Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset = 0);

    #ifdef STDLIB_ENABLE_FILE_STATS
        MUST_BE_OPEN [[nodiscard]] FileStats StatsGet() const;
        MUST_BE_OPEN void StatsReset();
    #endif

        MUST_BE_OPEN [[nodiscard]] FileOpenMode OpenMode() const;

        MUST_BE_OPEN [[nodiscard]] Result<FilePath> PNN() const;

        MUST_BE_OPEN [[nodiscard]] fileHandle OsFileDescriptor() const;

        MUST_BE_OPEN [[nodiscard]] fileHandle CloseAndGetOsFileDescriptor();

        MUST_BE_OPEN bool FlushInternal(); // flushes only internal buffers, doesn't flush system caches

        virtual void Close() override;
        [[nodiscard]] virtual bool IsOpened() const override;

        MUST_BE_OPEN virtual bool Read(void *RSTR target, ui32 len, ui32 *RSTR read = 0) override;
        MUST_BE_OPEN virtual bool Write(const void *source, ui32 len, ui32 *RSTR written = 0) override;

        MUST_BE_OPEN virtual bool Flush() override;

        [[nodiscard]] virtual bool IsBufferingSupported() const override;
        MUST_BE_OPEN virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) override;
        MUST_BE_OPEN [[nodiscard]] virtual std::pair<ui32, const void *> BufferGet() const override;

        [[nodiscard]] virtual bool IsSeekSupported() const override;

        MUST_BE_OPEN [[nodiscard]] virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) override;
        MUST_BE_OPEN virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) override;

        MUST_BE_OPEN [[nodiscard]] virtual Result<ui64> SizeGet() override;
        MUST_BE_OPEN virtual Error<> SizeSet(ui64 newSize) override;

        MUST_BE_OPEN [[nodiscard]] virtual FileProcModes::FileProcMode ProcMode() const override;
        MUST_BE_OPEN [[nodiscard]] virtual FileCacheModes::FileCacheMode CacheMode() const override;

    private:
        bool PerformFlush(bool isFlushSystemCaches);
        void FlushSystemCaches();
        bool WriteToFile(const void *source, ui32 len, ui32 *written);
        bool ReadFromFile(void *target, ui32 len, ui32 *read);
        bool CancelCachedRead();
        Result<i64> CurrentFileOffset() const;
    };

#undef MUST_BE_OPEN
}

#ifdef STDLIB_ENABLE_FILE_STATS
#pragma detect_mismatch("STDLIB_ENABLE_FILE_STATS", "1")
#else
#pragma detect_mismatch("STDLIB_ENABLE_FILE_STATS", "0")
#endif