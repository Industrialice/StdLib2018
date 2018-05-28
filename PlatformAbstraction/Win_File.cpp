#include <StdCoreLib.hpp>
#include "File.hpp"
#include <Windows.h>

using namespace StdLib;

namespace
{
    DWORD(WINAPI *StdLib_GetFinalPathNameByHandleW)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
}

extern NOINLINE Error<> StdLib_FileError();

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, uiw offset, FileCacheMode cacheMode, FileShareMode shareMode)
{
    Close();

    ASSUME(_handle == INVALID_HANDLE_VALUE);

    offset = std::min<uiw>(offset, iw_max);

    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = 0;
    DWORD dwFlagsAndAttributes = 0;
    DWORD dwShareMode = 0;

    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("Path is invalid");
    }

    if (!StdLib_GetFinalPathNameByHandleW)
    {
        _pnn = pnn.GetAbsolute();
    }

    ASSUME((procMode && FileProcMode::Read) || (procMode && FileProcMode::Write));

    if (procMode && FileProcMode::Write)
    {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    if (procMode && FileProcMode::Read)
    {
        dwDesiredAccess |= GENERIC_READ;
    }

    if (openMode == FileOpenMode::CreateIfNotExist)
    {
        dwCreationDisposition = OPEN_ALWAYS;
    }
    else if (openMode == FileOpenMode::CreateAlways || openMode == FileOpenMode::CreateNew)
    {
        if (offset > 0)
        {
            return DefaultError::InvalidArgument("'offset' can't be more than 0 when OpenMode::CreateAlways or OpenMode::CreateNew is used");
        }

        if (openMode == FileOpenMode::CreateAlways)
        {
            dwCreationDisposition = CREATE_ALWAYS;
        }
        else
        {
            dwCreationDisposition = CREATE_NEW;
        }
    }
    else
    {
        ASSUME(openMode == FileOpenMode::OpenExisting);
        dwCreationDisposition = OPEN_EXISTING;
    }

    if ((cacheMode && FileCacheMode::LinearRead) || (cacheMode && FileCacheMode::RandomRead))
    {
        if (cacheMode && (FileCacheMode::LinearRead + FileCacheMode::RandomRead))
        {
            return DefaultError::InvalidArgument("Both FileCacheMode::LinearRead and FileCacheMode::RandomRead are specified");
        }

        if (!(procMode && FileProcMode::Read))
        {
            return DefaultError::InvalidArgument("FileCacheMode::LinearRead or FileCacheMode::RandomRead only can be used when FileProcMode::Read is specified");
        }

        if (cacheMode && FileCacheMode::LinearRead)
        {
            dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
        }
        else
        {
            dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
        }
    }

    if (cacheMode && FileCacheMode::DisableSystemReadCache)
    {
        if (!(procMode && FileProcMode::Read))
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemReadCache only can be used when FileProcMode::Read is specified");
        }

        dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
    }

    if (cacheMode && FileCacheMode::DisableSystemWriteCache)
    {
        if (!(procMode && FileProcMode::Write))
        {
            return DefaultError::InvalidArgument("FileCacheMode::DisableSystemWriteCache only can be used when FileProcMode::Write is specified");
        }

        dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
    }

    if (shareMode && FileShareMode::Delete)
    {
        dwShareMode |= FILE_SHARE_DELETE;
    }
    if (shareMode && FileShareMode::Read)
    {
        if (procMode && FileProcMode::Write)
        {
            if (!(shareMode && FileShareMode::Write))
            {
                return DefaultError::InvalidArgument("FileShareMode::Read without FileShareMode::Write is not a valid sharable option for a file that is opened for write");
            }
        }
        dwShareMode |= FILE_SHARE_READ;
    }
    if (shareMode && FileShareMode::Write)
    {
        dwShareMode |= FILE_SHARE_WRITE;
    }

    HANDLE hfile = CreateFileW(pnn.PlatformPath().data(), dwDesiredAccess, dwShareMode, 0, dwCreationDisposition, dwFlagsAndAttributes, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        return StdLib_FileError();
    }

    if (offset > 0)
    {
        LARGE_INTEGER size;
        if (!GetFileSizeEx(hfile, &size))
        {
            BOOL result = CloseHandle(hfile);
            ASSUME(result);
            return StdLib_FileError();
        }
        offset = std::min<uiw>(offset, size.QuadPart);
        size.QuadPart = std::min<i64>(size.QuadPart, offset);
        if (!SetFilePointerEx(hfile, size, nullptr, FILE_BEGIN))
        {
            BOOL result = CloseHandle(hfile);
            ASSUME(result);
            return StdLib_FileError();
        }
        _offsetToStart = size.QuadPart;
    }
    else
    {
        _offsetToStart = 0;
    }

    _openMode = openMode;
    _procMode = procMode;
    _cacheMode = cacheMode;
    _handle = hfile;
    _bufferPos = 0;
#if ENABLE_FILE_STATS
    _stats = {};
#endif
    _readBufferCurrentSize = 0;
    _isOwningFileHandle = true;

    return DefaultError::Ok();
}

Error<> File::Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, uiw offset)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

Result<FilePath> File::PNN() const
{
    ASSUME(IsOpened());

    if (StdLib_GetFinalPathNameByHandleW)
    {
        wchar_t tempBuf[MaxPathLength];
        DWORD result = StdLib_GetFinalPathNameByHandleW(_handle, tempBuf, MaxPathLength - 1, FILE_NAME_NORMALIZED);
        if (result < 2 || result >= MaxPathLength)
        {
            return DefaultError::UnknownError("GetFinalPathNameByHandle failed");
        }
        return FilePath(tempBuf);
    }
    else
    {
        return _pnn;
    }
}

void File::Close()
{
    if (!IsOpened())
    {
        return;
    }
    Flush();
    if (_isOwningFileHandle)
    {
        BOOL result = CloseHandle(_handle);
        ASSUME(result == TRUE);
    }
    _handle = INVALID_HANDLE_VALUE;
    _bufferPos = 0;
    _readBufferCurrentSize = 0;
}

Result<i64> File::OffsetSet(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpened());

    DWORD moveMethod;

    if (!CancelCachedRead() || !File::Flush())
    {
        return DefaultError::UnknownError();
    }

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        if (offset < 0)
        {
            return DefaultError::InvalidArgument("Negative offset value cannot be used with FileOffsetMode::FromBegin");
        }
        moveMethod = FILE_BEGIN;
        offset += _offsetToStart;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        moveMethod = FILE_CURRENT;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        if (offset > 0)
        {
            return DefaultError::InvalidArgument("Positive offset value cannot be used with FileOffsetMode::FromEnd");
        }
        moveMethod = FILE_END;
    }

    LARGE_INTEGER move;
    move.QuadPart = offset;

    if (!SetFilePointerEx(_handle, move, &move, moveMethod))
    {
        return StdLib_FileError();
    }

    if (move.QuadPart < _offsetToStart)
    {
        move.QuadPart = _offsetToStart;
        if (!SetFilePointerEx(_handle, move, &move, FILE_BEGIN))
        {
            return StdLib_FileError();
        }
    }

    ASSUME(move.QuadPart >= _offsetToStart);
    return move.QuadPart - _offsetToStart;
}

Result<ui64> File::SizeGet()
{
    ASSUME(IsOpened());
    if (!File::Flush()) // flushing first because file pointer may not be at the end of the file, in that case we can't just return FileSize + BufferSize
    {
        return DefaultError::UnknownError();
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(_handle, &size))
    {
        return StdLib_FileError();
    }
    if (!_readBufferCurrentSize)
    {
        size.QuadPart += (i64)_bufferPos;
    }
    ASSUME(size.QuadPart >= _offsetToStart);
    return size.QuadPart - _offsetToStart;
}

Error<> File::SizeSet(ui64 newSize)
{
    ASSUME(IsOpened());

    if (!CancelCachedRead() || !File::Flush())
    {
        return DefaultError::UnknownError();
    }

    newSize += (ui64)_offsetToStart;
    if (newSize < (ui64)_offsetToStart) // overflow
    {
        newSize = ui64_max;
    }

    LARGE_INTEGER currentOffset;
    if (!SetFilePointerEx(_handle, {}, &currentOffset, FILE_CURRENT)) // retrieve the current offset
    {
        return StdLib_FileError();
    }
    ASSUME(currentOffset.QuadPart >= _offsetToStart);

    if (currentOffset.QuadPart != newSize)
    {
        LARGE_INTEGER newOffset;
        newOffset.QuadPart = newSize;
        if (!SetFilePointerEx(_handle, newOffset, nullptr, FILE_BEGIN))
        {
            return StdLib_FileError();
        }
    }

    if (!SetEndOfFile(_handle))
    {
        return StdLib_FileError();
    }

    if (!SetFilePointerEx(_handle, currentOffset, nullptr, FILE_BEGIN)) // reset the offset
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

void File::FlushSystemCaches()
{
    ASSUME(IsOpened());
    if (_procMode && FileProcMode::Write)
    {
        BOOL result = FlushFileBuffers(_handle);
        ASSUME(result);
    }
}

bool File::WriteToFile(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ASSUME(source || len == 0);

#if ENABLE_FILE_STATS
    ++_stats.writesToFileCount;
#endif

    DWORD wapiWritten;
    if (len && !WriteFile(_handle, source, len, &wapiWritten, 0))
    {
        if (written) *written = 0;
        return false;
    }

#if ENABLE_FILE_STATS
    _stats.bytesToFileWritten += len;
#endif

    if (written) *written = len;
    return true;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *read)
{
    ASSUME(IsOpened());
    ASSUME(target || len == 0);

#if ENABLE_FILE_STATS
    ++_stats.readsFromFileCount;
#endif

    DWORD wapiRead = 0;
    if (len && !ReadFile(_handle, target, len, &wapiRead, 0))
    {
        if (read) *read = 0;
        return false;
    }

#if ENABLE_FILE_STATS
    _stats.bytesFromFileRead += wapiRead;
#endif

    if (read) *read = wapiRead;
    return true;
}

NOINLINE bool File::CancelCachedRead()
{
    ASSUME(IsOpened());
    if (_bufferPos >= _readBufferCurrentSize)
    {
        ASSUME(_readBufferCurrentSize == 0 || _bufferPos == _readBufferCurrentSize);
        return true;
    }
    LONGLONG move = (LONGLONG)_bufferPos - (LONGLONG)_readBufferCurrentSize;
    ASSUME(move <= 0);
    LARGE_INTEGER quadMove;
    quadMove.QuadPart = move;
    BOOL result = SetFilePointerEx(_handle, quadMove, 0, FILE_CURRENT);
    _bufferPos = _readBufferCurrentSize = 0;
    return result != FALSE;
}

Result<i64> File::CurrentFileOffset() const
{
    LARGE_INTEGER currentOffset;
    if (!SetFilePointerEx(_handle, {}, &currentOffset, FILE_CURRENT))
    {
        return StdLib_FileError();
    }
    ASSUME(currentOffset.QuadPart >= _offsetToStart);
    return currentOffset.QuadPart;
}

namespace StdLib::FileInitialization
{
    void Initialize()
    {
        HMODULE k32 = GetModuleHandleA("kernel32.dll");
        if (!k32)
        {
            HARDBREAK;
            return;
        }
        *(uiw *)&StdLib_GetFinalPathNameByHandleW = (uiw)GetProcAddress(k32, "GetFinalPathNameByHandleW");
    }
}