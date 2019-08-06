#pragma once

#include "EnumCombinable.hpp"
#include "Result.hpp"
#include "TypeIdentifiable.hpp"

namespace StdLib
{
    struct FileProcModes
    {
        static constexpr struct FileProcMode : EnumCombinable<FileProcMode, ui8>
        {} Read = FileProcMode::Create(1 << 0),
            Write = FileProcMode::Create(1 << 1),
			ReadWrite = Read.Combined(Write);
    };

    // these constraints aren't strictly enforced, so you should not rely on them only
	// None: the file is exclusive and cannot be open for either read, or write
	// Read: the file can be open for read
	// Write: the file can be open for write
	// Delete: the file is allowed to be deleted
    struct FileShareModes
    {
        static constexpr struct FileShareMode : EnumCombinable<FileShareMode, ui8, true>
        {} None = FileShareMode::Create(0),
            Read = FileShareMode::Create(1 << 0),
            Write = FileShareMode::Create(1 << 1),
            Delete = FileShareMode::Create(1 << 2);
    };

    // mode                already exists        doesn't exist
    // ===                 ===                   ===
    // CreateIfNotExist    opens                 creates new & opens
    // CreateAlways        truncates & opens     creates new & opens
    // CreateNew           fails                 creates new & opens
    // OpenExisting        opens                 fails

    enum class FileOpenMode : ui8
    {
        CreateIfNotExist,
        CreateAlways,
        CreateNew,
        OpenExisting
    };

    // sometimes when you disable write cache you'll also get read cache disabled and vise versa
    struct FileCacheModes
    {
        static constexpr struct FileCacheMode : EnumCombinable<FileCacheMode, ui8, true>
        {} Default = FileCacheMode::Create(0),
            LinearRead = FileCacheMode::Create(1 << 0), /* requires ProcMode::Read, can't be used with RandomRead */
            LinearWrite = FileCacheMode::Create(1 << 1), /* requires ProcMode::Write, can't be used with RandomWrite */
            RandomRead = FileCacheMode::Create(1 << 2), /* requires ProcMode::Read, can't be used with LinearRead */
            RandomWrite = FileCacheMode::Create(1 << 3), /* requires ProcMode::Write, can't be used with LinearWrite */
            DisableSystemWriteCache = FileCacheMode::Create(1 << 4), /* requires ProcMode::Write */
            DisableSystemReadCache = FileCacheMode::Create(1 << 5) /* requires ProcMode::Read */;
    };

    // position in file: ..#...
    // Offset(FileOffsetMode::FromBegin) == 2
    // Offset(FileOffsetMode::FromCurrent) == 0 (always zero)
    // Offset(FileOffsetMode::FromEnd) == -3

    enum class FileOffsetMode : ui8
    {
        FromBegin,
        FromCurrent,
        FromEnd
    };

	// a very generic abstraction that can be used with any data structure, but it might be less
	// efficient than less generic abstractions like IMemoryStream
    struct NOVTABLE IFile
    {
        using bufferType = std::unique_ptr<std::byte, void(*)(std::byte *)>;

        virtual ~IFile() = default;

		[[nodiscard]] virtual TypeId Type() const = 0;

        virtual void Close() = 0;
		[[nodiscard]] virtual bool IsOpen() const = 0;

        virtual bool Read(void *RSTR target, ui32 len, ui32 *RSTR read = nullptr) = 0;
        virtual bool Write(const void *RSTR source, ui32 len, ui32 *RSTR written = nullptr) = 0;

        virtual bool Flush() = 0;
		[[nodiscard]] virtual bool IsBufferingSupported() const = 0;
        virtual bool Buffer(ui32 size, bufferType &&buffer = {nullptr, nullptr}) = 0; // will reject this call if buffering isn't supported, pass nullptr as a buffer to use an auto allocated buffer, pass 0 as size to disable buffering
		[[nodiscard]] virtual std::pair<ui32, const void *> Buffer() const = 0; // will return nullptr if there's no buffer

		[[nodiscard]] virtual bool IsSeekSupported() const = 0;
		[[nodiscard]] virtual Result<i64> Offset(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) = 0;
        virtual Result<i64> Offset(FileOffsetMode offsetMode, i64 offset) = 0; // negative offset with FileOffsetMode::Begin and positive offset with FileOffsetMode::FromEnd is undefined behavior

		[[nodiscard]] virtual Result<ui64> Size() = 0;
        virtual Error<> Size(ui64 newSize) = 0; // if the file is extended, the extended part's content is undefined

		[[nodiscard]] virtual FileProcModes::FileProcMode ProcMode() const = 0;
		[[nodiscard]] virtual FileCacheModes::FileCacheMode CacheMode() const = 0;

        [[nodiscard]] explicit operator bool() const
        {
            return IsOpen();
        }
    };
}