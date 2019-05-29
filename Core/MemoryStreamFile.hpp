#pragma once

#include "IMemoryStream.hpp"
#include "IFile.hpp"

WARNING_PUSH
WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW
WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED

namespace StdLib
{
    class MemoryStreamFile final : public IFile, public TypeIdentifiable<MemoryStreamFile>
    {
        IMemoryStream *_stream = nullptr;
        ui64 _offset;  //  _startOffset + current offset
        ui64 _startOffset;  //  can be non-zero in append mode
        FileProcModes::FileProcMode _procMode = FileProcModes::Read;

    public:
        MemoryStreamFile() = default;
        MemoryStreamFile(IMemoryStream &stream, FileProcModes::FileProcMode procMode, ui64 offset = 0, Error<> *error = nullptr);
        MemoryStreamFile(MemoryStreamFile &&source) noexcept;
        MemoryStreamFile &operator = (MemoryStreamFile &&source) noexcept;
        Error<> Open(IMemoryStream &stream, FileProcModes::FileProcMode procMode, ui64 offset = 0);

		[[nodiscard]] virtual TypeId Type() const override;

        virtual void Close() override;
        [[nodiscard]] virtual bool IsOpen() const override;

		[[must_be_open]] virtual bool Read(void *RSTR target, ui32 len, ui32 *RSTR read = 0) override;
		[[must_be_open]] virtual bool Write(const void *source, ui32 len, ui32 *RSTR written = 0) override;

		[[must_be_open]] virtual bool Flush() override;
		[[nodiscard]] virtual bool IsBufferingSupported() const override;
        virtual bool Buffer(ui32 size, bufferType &&buffer = {nullptr, nullptr}) override;
		[[nodiscard]] virtual std::pair<ui32, const void *> Buffer() const override;

		[[nodiscard]] virtual bool IsSeekSupported() const override;
		[[must_be_open]] [[nodiscard]] virtual Result<i64> Offset(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) override;
		[[must_be_open]] virtual Result<i64> Offset(FileOffsetMode offsetMode, i64 offset) override;

		[[must_be_open]] [[nodiscard]] virtual Result<ui64> Size() override;
		[[must_be_open]] virtual Error<> Size(ui64 newSize) override;

		[[must_be_open]] [[nodiscard]] virtual FileProcModes::FileProcMode ProcMode() const override;
		[[must_be_open]] [[nodiscard]] virtual FileCacheModes::FileCacheMode CacheMode() const override;
    };
}

WARNING_POP