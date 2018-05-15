#pragma once

#include "Result.hpp"
#include "GenericFuncs.hpp"

namespace StdLib
{
    enum class FileProcMode : ui8
    {
        Read = Funcs::BitPos(1),
        Write = Funcs::BitPos(2),
        WriteAppend = Funcs::BitPos(3) | Funcs::BitPos(2)  //  makes existing part of the file virtually invisible( isn't reported with Size calls, isn't accessible through offset sets )
    };

    // mode                already exists        doesn't exist
    // ***                 ***                   ***
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

    // sometimes disabling write cache you'll also disable read cache and vise versa
    enum class FileCacheMode : ui8
    {
        Default = 0,
        LinearRead = Funcs::BitPos(1), // requires ProcMode::Read, can't be used with RandomRead
        LinearWrite = Funcs::BitPos(2), // requires ProcMode::Write, can't be used with RandomWrite
        RandomRead = Funcs::BitPos(3), // requires ProcMode::Read, can't be used with LinearRead
        RandomWrite = Funcs::BitPos(4), // requires ProcMode::Write, can't be used with LinearWrite
        DisableSystemWriteCache = Funcs::BitPos(5), // requires ProcMode::Write
        DisableSystemReadCache = Funcs::BitPos(6) // requires ProcMode::Read
    };

    // file content: ....#.....
    // OffsetGet(FileOffsetMode::FromBegin) == 4
    // OffsetGet(FileOffsetMode::FromCurrent) == 0 (always zero)
    // OffsetGet(FileOffsetMode::FromEnd) == -5

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
        virtual bool BufferSet(ui32 size, bufferType &&buffer = {nullptr, nullptr}) = 0; // will reject this call if buffering isn't supported, pass null as a buffer to use an auto allocated buffer, pass 0 as a size to disable buffering
        virtual std::pair<ui32, const void *> BufferGet() const = 0; //  will return nullptr if there's no buffer

        virtual bool IsSeekSupported() const = 0;
        virtual Result<i64> OffsetGet(FileOffsetMode offsetMode = FileOffsetMode::FromBegin) = 0;
        virtual Result<i64> OffsetSet(FileOffsetMode offsetMode, i64 offset) = 0;

        virtual Result<ui64> SizeGet() const = 0;
        virtual Error<> SizeSet(ui64 newSize) = 0; // if the file is extended, the extended part's content is undefined

        virtual FileProcMode ProcMode() const = 0;
        virtual FileCacheMode CacheMode() const = 0;
    };

    ENUM_COMBINABLE(FileProcMode, ui8);
    ENUM_COMBINABLE(FileCacheMode, ui8);
}