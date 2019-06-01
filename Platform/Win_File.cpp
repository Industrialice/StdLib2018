#include "_PreHeader.hpp"
#include "File.hpp"

using namespace StdLib;

namespace
{
	DWORD(WINAPI *StdLib_GetFinalPathNameByHandleW)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
}

extern NOINLINE Error<> StdLib_FileError();

Error<> File::Open(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, FileShareModes::FileShareMode shareMode)
{
    Close();

    ASSUME(_handle == INVALID_HANDLE_VALUE);

    offset = std::min<ui64>(offset, i64_max);

    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = 0;
    DWORD dwFlagsAndAttributes = 0;
    DWORD dwShareMode = 0;

    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("Path is invalid");
    }

	#ifdef PLATFORM_WINXP
		if (!StdLib_GetFinalPathNameByHandleW)
		{
			_pnn = pnn.GetAbsolute();
		}
	#endif

    ASSUME(procMode.Contains(FileProcModes::Read) || procMode.Contains(FileProcModes::Write));

    if (procMode.Contains(FileProcModes::Write))
    {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    if (procMode.Contains(FileProcModes::Read))
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

    if (cacheMode.Contains(FileCacheModes::LinearRead) || cacheMode.Contains(FileCacheModes::RandomRead))
    {
        if (cacheMode.Contains(FileCacheModes::LinearRead.Combined(FileCacheModes::RandomRead)))
        {
            return DefaultError::InvalidArgument("Both FileCacheModes::LinearRead and FileCacheModes::RandomRead are specified");
        }

        if (!procMode.Contains(FileProcModes::Read))
        {
            return DefaultError::InvalidArgument("FileCacheModes::LinearRead or FileCacheModes::RandomRead only can be used when FileProcModes::Read is specified");
        }

        if (cacheMode.Contains(FileCacheModes::LinearRead))
        {
            dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
        }
        else
        {
            dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
        }
    }

    if (cacheMode.Contains(FileCacheModes::DisableSystemReadCache))
    {
        if (!procMode.Contains(FileProcModes::Read))
        {
            return DefaultError::InvalidArgument("FileCacheModes::DisableSystemReadCache only can be used when FileProcModes::Read is specified");
        }

        dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
    }

    if (cacheMode.Contains(FileCacheModes::DisableSystemWriteCache))
    {
        if (!procMode.Contains(FileProcModes::Write))
        {
            return DefaultError::InvalidArgument("FileCacheModes::DisableSystemWriteCache only can be used when FileProcModes::Write is specified");
        }

        dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
    }

    if (shareMode.Contains(FileShareModes::Delete))
    {
        dwShareMode |= FILE_SHARE_DELETE;
    }
    if (shareMode.Contains(FileShareModes::Read))
    {
        if (procMode.Contains(FileProcModes::Write))
        {
            if (!shareMode.Contains(FileShareModes::Write))
            {
                return DefaultError::InvalidArgument("FileShareModes::Read without FileShareModes::Write is not a valid sharable option for a file that is open for write");
            }
        }
        dwShareMode |= FILE_SHARE_READ;
    }
    if (shareMode.Contains(FileShareModes::Write))
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
        offset = std::min<ui64>(offset, size.QuadPart);
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
#if STDLIB_ENABLE_FILE_STATS
    _stats = {};
#endif
    _readBufferCurrentSize = 0;
    _isOwningFileHandle = true;

    return DefaultError::Ok();
}

Error<> File::Open(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

Result<FilePath> File::PNN() const
{
    ASSUME(IsOpen());

	#ifdef PLATFORM_WINXP
		if (StdLib_GetFinalPathNameByHandleW)
		{
	#endif
			wchar_t tempBuf[MaxPathLength];
			DWORD result = StdLib_GetFinalPathNameByHandleW(_handle, tempBuf, MaxPathLength - 1, FILE_NAME_NORMALIZED);
			if (result < 2 || result >= MaxPathLength)
			{
				return DefaultError::UnknownError("GetFinalPathNameByHandle failed");
			}
			return FilePath(tempBuf);
	#ifdef PLATFORM_WINXP
		}
		else
		{
			return _pnn;
		}
	#endif
}

void File::Close()
{
    if (!IsOpen())
    {
        return;
    }
    FlushWriteBuffers(false);
    if (_isOwningFileHandle)
    {
        BOOL result = CloseHandle(_handle);
        ASSUME(result == TRUE);
    }
    _handle = INVALID_HANDLE_VALUE;
    _bufferPos = 0;
    _readBufferCurrentSize = 0;
}

Result<i64> File::Offset(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(IsOpen());

    DWORD moveMethod;

    if (!CancelCachedRead() || !FlushWriteBuffers(false))
    {
        return DefaultError::UnknownError("Flushing buffers failed");
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

Result<ui64> File::Size()
{
    ASSUME(IsOpen());
    if (!FlushWriteBuffers(false)) // flushing first because file pointer may not be at the end of the file, in that case we can't just return FileSize + BufferSize
    {
        return DefaultError::UnknownError("Flushing write buffers failed");
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(_handle, &size))
    {
        return StdLib_FileError();
    }
    if (!_readBufferCurrentSize)
    {
        size.QuadPart += static_cast<i64>(_bufferPos);
    }
    ASSUME(size.QuadPart >= _offsetToStart);
    return size.QuadPart - _offsetToStart;
}

Error<> File::Size(ui64 newSize)
{
    ASSUME(IsOpen());

    if (!CancelCachedRead() || !FlushWriteBuffers(false))
    {
        return DefaultError::UnknownError("Flushing buffers failed");
    }

    newSize += static_cast<ui64>(_offsetToStart);
    if (newSize < static_cast<ui64>(_offsetToStart)) // overflow
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
    ASSUME(IsOpen());
    if (_procMode.Contains(FileProcModes::Write))
    {
        BOOL result = FlushFileBuffers(_handle);
        ASSUME(result);
    }
}

bool File::WriteToFile(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpen());
    ASSUME(source || len == 0);

    DWORD wapiWritten;
	BOOL result = WriteFile(_handle, source, len, &wapiWritten, 0);

	#if STDLIB_ENABLE_FILE_STATS
		++_stats.writesToFileCount;
		_stats.bytesToFileWritten += wapiWritten;
	#endif

    if (written) *written = wapiWritten;

    return result;
}

bool File::ReadFromFile(void *target, ui32 len, ui32 *read)
{
    ASSUME(IsOpen());
    ASSUME(target || len == 0);

    DWORD wapiRead;
	BOOL result = ReadFile(_handle, target, len, &wapiRead, 0);

	#if STDLIB_ENABLE_FILE_STATS
		++_stats.readsFromFileCount;
		_stats.bytesFromFileRead += wapiRead;
	#endif

	if (read) *read = wapiRead;

    return result;
}

bool File::CancelCachedRead()
{
    ASSUME(IsOpen());
    if (_bufferPos >= _readBufferCurrentSize)
    {
        ASSUME(_readBufferCurrentSize == 0 || _bufferPos == _readBufferCurrentSize);
        return true;
    }
    LONGLONG move = static_cast<LONGLONG>(_bufferPos) - static_cast<LONGLONG>(_readBufferCurrentSize);
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
		#ifdef PLATFORM_WINXP
			HMODULE k32 = GetModuleHandleA("kernel32.dll");
			if (!k32)
			{
				HARDBREAK;
				return;
			}
			using type = decltype(StdLib_GetFinalPathNameByHandleW);
			StdLib_GetFinalPathNameByHandleW = static_cast<type>(GetProcAddress(k32, "GetFinalPathNameByHandleW")); // exists since Vista
		#else
			StdLib_GetFinalPathNameByHandleW = GetFinalPathNameByHandleW;
		#endif
    }
}