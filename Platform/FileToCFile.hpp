#pragma once

#include "FilePath.hpp"
#include "IMemoryStream.hpp"
#include "IFile.hpp"

WARNING_PUSH
WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW
WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED

namespace StdLib
{
	class FileToCFile final : public IFile, public TypeIdentifiable<FileToCFile>
    {
        FILE *_file = nullptr;
        FileOpenMode _openMode;
        FileProcModes::FileProcMode _procMode = FileProcModes::Read;
        FileCacheModes::FileCacheMode _cacheMode;
        i64 _offsetToStart; // used only when you're using ProcMode::Append, the file will be opened as usual, then the offset will be added so you can't work with the existing part of the file
        ui32 _bufferSize;
        bufferType _customBufferPtr = {nullptr, nullptr};

    public:
        ~FileToCFile();
        FileToCFile() = default;
        FileToCFile(const FilePath &path, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None, Error<> *error = 0);

        FileToCFile(FileToCFile &&source) noexcept;
        FileToCFile &operator = (FileToCFile &&source) noexcept;

        Error<> Open(const FilePath &path, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None);

		[[must_be_open]] [[nodiscard]] FileOpenMode OpenModeGet() const;

		[[nodiscard]] virtual TypeId Type() const override;

        virtual void Close() override;
		[[nodiscard]] virtual bool IsOpen() const override;

		[[must_be_open]] virtual bool Read(void *RSTR target, ui32 len, ui32 *RSTR read = 0) override;
		[[must_be_open]] virtual bool Write(const void *source, ui32 len, ui32 *RSTR written = 0) override;

		[[must_be_open]] [[nodiscard]] virtual bool Flush() override;
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
    };
}

WARNING_POP