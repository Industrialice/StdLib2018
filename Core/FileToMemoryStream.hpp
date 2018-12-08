#pragma once

#include "IMemoryStream.hpp"
#include "IFile.hpp"

namespace StdLib
{
    class FileToMemoryStream final : public IFile
    {
        IMemoryStream *_stream = nullptr;
        uiw _offset;  //  _startOffset + current offset
        uiw _startOffset;  //  can be non-zero in append mode
        FileProcModes::FileProcMode _procMode = FileProcModes::Read;

    public:
        FileToMemoryStream() = default;
        FileToMemoryStream(IMemoryStream &stream, FileProcModes::FileProcMode procMode, uiw offset = 0, Error<> *error = nullptr);
        FileToMemoryStream(FileToMemoryStream &&source) noexcept;
        FileToMemoryStream &operator = (FileToMemoryStream &&source) noexcept;
        Error<> Open(IMemoryStream &stream, FileProcModes::FileProcMode procMode, uiw offset = 0);

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

        virtual FileProcModes::FileProcMode ProcMode() const override;
        virtual FileCacheModes::FileCacheMode CacheMode() const override;
    };
}