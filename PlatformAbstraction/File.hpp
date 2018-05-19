#pragma once

#include "FilePath.hpp"
#include <IFile.hpp>

namespace StdLib
{
#define MUST_BE_OPENED

    class File : public IFile
    {
    public:

    #ifdef ENABLE_FILE_STATS
        struct FileStats
        {
            using counter_t = ui64;
            counter_t writesToBufferCount;
            counter_t writesToFileCount;
            counter_t readsFromBufferCount;
            counter_t readsFromFileCount;
            counter_t bytesFromBufferRead;
            counter_t bytesFromFileReaded;
            counter_t bytesToBufferWritten;
            counter_t bytesToFileWritten;
            counter_t bufferedWrites;
            counter_t unbufferedWrites;
            counter_t bufferedReads;
            counter_t unbufferedReads;
        };
    #endif

    private:
        fileHandle _handle = fileHandle_undefined;
    #ifdef ENABLE_FILEIO_STATS
        FileStats _stats{};
    #endif
        ui64 _offsetToStart; // used only when you're using ProcMode::Append, the file will be opened as usual, then the offset will be added so you can't work with the existing part of the file
        bufferType _internalBuffer = {nullptr, nullptr};
        ui32 _bufferSize = 0;
        ui32 _bufferPos = 0;
        ui32 _readBufferCurrentSize = 0; // can be lower than bufferSize if, for example, EOF is reached, 0 if not reading
        bool _isOwningFileHandle = true;
        FileOpenMode _openMode;
        FileProcMode _procMode;
        FileCacheMode _cacheMode;

    #ifdef PLATFORM_WINDOWS
        FilePath _pnn; // used only on WindowsXP where you can't get PNN from the file handle
    #endif

    public:
        ~File();
        File() = default;
        File(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode = FileCacheMode::Default, FileShareMode shareMode = FileShareMode::None, Error<> *error = nullptr);
        File(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, Error<> *error = nullptr);
        File(File &&other);
        File &operator = (File &&other);
        [[nodiscard]] Error<> Open(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode = FileCacheMode::Default, FileShareMode shareMode = FileShareMode::None);
        [[nodiscard]] Error<> Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership);
    #ifdef ENABLE_FILEIO_STATS
        MUST_BE_OPENED [[nodiscard]] FileStats StatsGet() const;
        MUST_BE_OPENED void StatsReset();
    #endif
        MUST_BE_OPENED [[nodiscard]] FileOpenMode OpenMode() const;
        MUST_BE_OPENED [[nodiscard]] Result<FilePath> PNN() const;
        MUST_BE_OPENED [[nodiscard]] fileHandle OsFileDescriptor() const;
        MUST_BE_OPENED [[nodiscard]] fileHandle CloseAndGetOsFileDescriptor();
        virtual void Close() override;
        [[nodiscard]] virtual bool IsOpened() const override;
        MUST_BE_OPENED virtual bool Read(void *target, ui32 len, ui32 *read = 0) override;
        MUST_BE_OPENED virtual bool Write(const void *source, ui32 len, ui32 *written = 0) override;
        MUST_BE_OPENED virtual bool Flush() override;
        virtual bool IsBufferingSupported() const override;
        virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) override;
        virtual std::pair<ui32, const void *> BufferGet() const override;
        virtual bool IsSeekSupported() const override;
        MUST_BE_OPENED virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) override;
        MUST_BE_OPENED virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) override;
        MUST_BE_OPENED virtual Result<ui64> SizeGet() override;
        MUST_BE_OPENED virtual Error<> SizeSet(ui64 newSize) override;
        MUST_BE_OPENED virtual FileProcMode ProcMode() const override;
        MUST_BE_OPENED virtual FileCacheMode CacheMode() const override;

    private:
        bool WriteToFile(const void *source, ui32 len, ui32 *written);
        bool ReadFromFile(void *target, ui32 len, ui32 *read);
        bool CancelCachedRead();
    };

#undef MUST_BE_OPENED
}