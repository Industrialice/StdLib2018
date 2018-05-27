#pragma once

#include "FilePath.hpp"
#include "IMemoryStream.hpp"
#include "IFile.hpp"

namespace StdLib
{
    class FileToCFile final : public IFile
    {
        FILE *_file = nullptr;
        FileOpenMode _openMode;
        FileProcMode _procMode;
        FileCacheMode _cacheMode;
        i64 _offsetToStart; // used only when you're using ProcMode::Append, the file will be opened as usual, then the offset will be added so you can't work with the existing part of the file
        ui32 _bufferSize;
        bufferType _customBufferPtr = {nullptr, nullptr};

    public:
        ~FileToCFile();
        FileToCFile() = default;
        FileToCFile(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, uiw offset = 0, FileCacheMode cacheMode = FileCacheMode::Default, FileShareMode shareMode = FileShareMode::None, Error<> *error = 0);

        FileToCFile(FileToCFile &&source);
        FileToCFile &operator = (FileToCFile &&source);

        Error<> Open(const FilePath &path, FileOpenMode openMode, FileProcMode procMode, uiw offset = 0, FileCacheMode cacheMode = FileCacheMode::Default, FileShareMode shareMode = FileShareMode::None);

        virtual void Close() override;
        virtual bool IsOpened() const override;

        virtual bool Read(void *target, ui32 len, ui32 *read = 0) override;
        virtual bool Write(const void *source, ui32 len, ui32 *written = 0) override;

        virtual bool Flush() override;
        virtual bool IsBufferingSupported() const override;
        virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) override;
        virtual std::pair<ui32, const void *> BufferGet() const override;

        virtual bool IsSeekSupported() const override;
        virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) override;
        virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) override;

        virtual Result<ui64> SizeGet() override;
        virtual Error<> SizeSet(ui64 newSize) override;

        virtual FileProcMode ProcMode() const override;
        virtual FileCacheMode CacheMode() const override;

        FileOpenMode OpenModeGet() const;
    };
}