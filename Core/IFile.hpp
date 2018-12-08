#pragma once

#include "Result.hpp"
#include "GenericFuncs.hpp"
#include "EnumCombinable.hpp"

namespace StdLib
{
    struct FileProcModes
    {
        static constexpr struct FileProcMode : EnumCombinable<FileProcMode, ui8>
        {} Read = FileProcMode::Create(1 << 0),
            Write = FileProcMode::Create(1 << 1);
    };

    // these constraints aren't strictly enforced, so you should not rely on them only
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
    // OffsetGet(FileOffsetMode::FromBegin) == 2
    // OffsetGet(FileOffsetMode::FromCurrent) == 0 (always zero)
    // OffsetGet(FileOffsetMode::FromEnd) == -3

    enum class FileOffsetMode : ui8
    {
        FromBegin,
        FromCurrent,
        FromEnd
    };

    struct IFile
    {
        using bufferType = std::unique_ptr<ui8, void(*)(ui8 *)>;

        virtual ~IFile() = default;

        virtual void Close() = 0;
        virtual bool IsOpened() const = 0;

        virtual bool Read(void *target, ui32 len, ui32 *read = 0) = 0;
        virtual bool Write(const void *source, ui32 len, ui32 *written = 0) = 0;

        virtual bool Flush() = 0;
        virtual bool IsBufferingSupported() const = 0;
        virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) = 0; // will reject this call if buffering isn't supported, pass nullptr as a buffer to use an auto allocated buffer, pass 0 as size to disable buffering
        virtual std::pair<ui32, const void *> BufferGet() const = 0; // will return nullptr if there's no buffer

        virtual bool IsSeekSupported() const = 0;
        virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) = 0;
        virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) = 0; // negative offset with FileOffsetMode::Begin and positive offset with FileOffsetMode::FromEnd is undefined behavior

        virtual Result<ui64> SizeGet() = 0;
        virtual Error<> SizeSet(ui64 newSize) = 0; // if the file is extended, the extended part's content is undefined

        virtual FileProcModes::FileProcMode ProcMode() const = 0;
        virtual FileCacheModes::FileCacheMode CacheMode() const = 0;

        [[nodiscard]] explicit operator bool() const
        {
            return IsOpened();
        }
    };
}