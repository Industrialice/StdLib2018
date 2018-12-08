#pragma once

#include "FilePath.hpp"
#include "IMemoryStream.hpp"
#include "IFile.hpp"

namespace StdLib
{
	#define MUST_BE_OPEN

	class FileToCFile final : public IFile
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

        FileToCFile(FileToCFile &&source);
        FileToCFile &operator = (FileToCFile &&source);

        Error<> Open(const FilePath &path, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset = 0, FileCacheModes::FileCacheMode cacheMode = FileCacheModes::Default, FileShareModes::FileShareMode shareMode = FileShareModes::None);

		MUST_BE_OPEN[[nodiscard]] FileOpenMode OpenModeGet() const;

        virtual void Close() override;
		[[nodiscard]] virtual bool IsOpened() const override;

		MUST_BE_OPEN virtual bool Read(void *target, ui32 len, ui32 *read = 0) override;
		MUST_BE_OPEN virtual bool Write(const void *source, ui32 len, ui32 *written = 0) override;

		MUST_BE_OPEN [[nodiscard]] virtual bool Flush() override;
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
    };

	#undef MUST_BE_OPEN
}