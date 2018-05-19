#include <StdCoreLib.hpp>
#include "File.hpp"
#include <Windows.h>

using namespace StdLib;

namespace
{
    DWORD(WINAPI *StdLib_GetFinalPathNameByHandleW)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
}

extern NOINLINE Error<> StdLib_FileError();

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, FileCacheMode cacheMode, FileShareMode shareMode)
{
    Close();

    ASSUME(_handle == INVALID_HANDLE_VALUE);

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
        _pnn = pnn.ToAbsolute();
    }

    if (!(procMode && FileProcMode::Read) && !(procMode && FileProcMode::Write))
    {
        return DefaultError::InvalidArgument("Neither read, nor write proc mode was requested");
    }

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
        if (procMode && FileProcMode::WriteAppend)
        {
            return DefaultError::InvalidArgument("FileProcMode::WriteAppend can't be used with OpenMode::CreateAlways or OpenMode::CreateNew");
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

    if (procMode && FileProcMode::WriteAppend)
    {
        LARGE_INTEGER curPos = {};
        if (!SetFilePointerEx(hfile, {}, &curPos, FILE_END))
        {
            BOOL result = CloseHandle(hfile);
            ASSUME(result);
            return StdLib_FileError();
        }
        _offsetToStart = curPos.QuadPart;
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
#ifdef ENABLE_FILE_STATS
    _stats = {};
#endif
    _readBufferCurrentSize = 0;
    _isOwningFileHandle = true;

    return DefaultError::Ok();
}

Error<> File::Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership)
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

Result<i64> File::OffsetGet(FileOffsetMode offsetMode)
{
    ASSUME(IsOpened());

    if (offsetMode == FileOffsetMode::FromEnd)
    {
        if (!CancelCachedRead() || !File::Flush())
        {
            return DefaultError::UnknownError();
        }
    }

    LARGE_INTEGER pos;
    if (!SetFilePointerEx(_handle, LARGE_INTEGER(), &pos, FILE_CURRENT))
    {
        return StdLib_FileError();
    }
    ASSUME(pos.QuadPart >= (i64)_offsetToStart);
    LONGLONG offsetFromBegin = pos.QuadPart - _offsetToStart;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        ASSUME(_bufferPos == 0 || _internalBuffer.get());
        if (_readBufferCurrentSize)
        {
            offsetFromBegin -= (LONGLONG)(_readBufferCurrentSize - _bufferPos);
        }
        else
        {
            offsetFromBegin += (LONGLONG)_bufferPos;
        }

        return offsetFromBegin;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);

        auto fileSize = File::SizeGet();
        if (!fileSize)
        {
            return fileSize.GetError();
        }

        return offsetFromBegin - fileSize.Unwrap();
    }
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
        moveMethod = FILE_END;
    }

    LARGE_INTEGER move;
    move.QuadPart = offset;

    if (!SetFilePointerEx(_handle, move, &move, moveMethod))
    {
        return StdLib_FileError();
    }

    if (_procMode && FileProcMode::WriteAppend)
    {
        if (offsetMode != FileOffsetMode::FromBegin)
        {
            if (move.QuadPart < (i64)_offsetToStart)
            {
                move.QuadPart = _offsetToStart;
                if (!SetFilePointerEx(_handle, move, &move, FILE_BEGIN))
                {
                    return StdLib_FileError();
                }
            }
        }
        ASSUME(move.QuadPart >= (i64)_offsetToStart);
        move.QuadPart -= _offsetToStart;
    }

    return move.QuadPart;
}

Result<ui64> File::SizeGet()
{
    ASSUME(IsOpened());
    if (!File::Flush())
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
    ASSUME(size.QuadPart >= (i64)_offsetToStart);
    return size.QuadPart - _offsetToStart;
}

Error<> File::SizeSet(ui64 newSize)
{
    ASSUME(IsOpened());

    if (!CancelCachedRead() || !File::Flush())
    {
        return DefaultError::UnknownError();
    }

    newSize += _offsetToStart;
    if (newSize < _offsetToStart) // overflow
    {
        newSize = ui64_max;
    }

    LARGE_INTEGER currentOffset;
    if (!SetFilePointerEx(_handle, {}, &currentOffset, FILE_CURRENT)) // retrieve the current offset
    {
        return StdLib_FileError();
    }
    ASSUME(currentOffset.QuadPart >= (i64)_offsetToStart);

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

bool File::WriteToFile(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ASSUME(source || len == 0);

#ifdef ENABLE_FILE_STATS
    ++_stats.writesToFileCount;
#endif

    DWORD wapiWritten;
    if (len && !WriteFile(_handle, source, len, &wapiWritten, 0))
    {
        if (written) *written = 0;
        return false;
    }

#ifdef ENABLE_FILE_STATS
    _stats.bytesToFileWritten += len;
#endif

    if (written) *written = len;
    return true;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *read)
{
    ASSUME(IsOpened());
    ASSUME(target || len == 0);

#ifdef ENABLE_FILE_STATS
    ++_stats.readsFromFileCount;
#endif

    DWORD wapiRead = 0;
    if (len && !ReadFile(_handle, target, len, &wapiRead, 0))
    {
        return false;
    }

#ifdef ENABLE_FILE_STATS
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