#include <StdCoreLib.hpp>
#include "File.hpp"

using namespace StdLib;

File::~File()
{
    Close();
}

File::File(const FilePath &pnn, FileOpenMode openMode, FileProcMode procMode, ui64 offset, FileCacheMode cacheMode, FileShareMode shareMode, Error<> *error)
{
    Error<> tempError = File::Open(pnn, openMode, procMode, offset, cacheMode, shareMode);
    if (error) *error = tempError;
}

File::File(fileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset, Error<> *error)
{
    Error<> tempError = File::Open(osFileDescriptor, isGettingFileDescriptorOwnership, offset);
    if (error) *error = tempError;
}

File::File(File &&other)
{
    _handle = other._handle;
    other._handle = fileHandle_undefined;
    _internalBuffer = std::move(other._internalBuffer);
    _bufferSize = other._bufferSize;
    _bufferPos = other._bufferPos;
    _readBufferCurrentSize = other._readBufferCurrentSize;
    _offsetToStart = other._offsetToStart;
    _isOwningFileHandle = other._isOwningFileHandle;
    _openMode = other._openMode;
    _procMode = other._procMode;
    _cacheMode = other._cacheMode;
#if ENABLE_FILE_STATS
    _stats = other._stats;
#endif
#ifdef PLATFORM_WINDOWS
    _pnn = std::move(other._pnn);
#endif
}

File &File::operator = (File &&other)
{
    File::Close();

    _handle = other._handle;
    other._handle = fileHandle_undefined;
    _internalBuffer = std::move(other._internalBuffer);
    _bufferSize = other._bufferSize;
    _bufferPos = other._bufferPos;
    _readBufferCurrentSize = other._readBufferCurrentSize;
    _offsetToStart = other._offsetToStart;
    _isOwningFileHandle = other._isOwningFileHandle;
    _openMode = other._openMode;
    _procMode = other._procMode;
    _cacheMode = other._cacheMode;
#if ENABLE_FILE_STATS
    _stats = other._stats;
#endif
#ifdef PLATFORM_WINDOWS
    _pnn = std::move(other._pnn);
#endif

    return *this;
}

#if ENABLE_FILE_STATS
auto File::StatsGet() const -> FileStats
{
    ASSUME(IsOpened());
    return _stats;
}

void File::StatsReset()
{
    ASSUME(IsOpened());
    _stats = {};
}
#endif

FileOpenMode File::OpenMode() const
{
    ASSUME(IsOpened());
    return _openMode;
}

fileHandle File::OsFileDescriptor() const
{
    ASSUME(IsOpened());
    return _handle;
}

fileHandle File::CloseAndGetOsFileDescriptor()
{
    ASSUME(IsOpened());
    _isOwningFileHandle = false;
    fileHandle handle = _handle;
    File::Close();
    return handle;
}

bool File::IsOpened() const
{
    return _handle != fileHandle_undefined;
}

NOINLINE bool File::Read(void *target, ui32 len, ui32 *read)
{
    ASSUME(IsOpened());
    ASSUME(target || len == 0);
    ASSUME(_procMode && FileProcMode::Read);

    auto readFromBuffer = [this](void *target, ui32 len)
    {
        memcpy(target, _internalBuffer.get() + _bufferPos, len);
        _bufferPos += len;
    #if ENABLE_FILE_STATS
        ++_stats.readsFromBufferCount;
        _stats.bytesFromBufferRead += len;
    #endif
    };

    if (read) *read = 0;

    if (_bufferSize)
    {
    #if ENABLE_FILE_STATS
        ++_stats.bufferedReads;
    #endif

        if (!File::Flush())
        {
            return false;
        }

        ASSUME(_bufferPos <= _readBufferCurrentSize);

        if (_readBufferCurrentSize - _bufferPos < len)
        {
            ui32 cpyLen = _readBufferCurrentSize - _bufferPos;
            readFromBuffer(target, cpyLen);
            len -= cpyLen;
            target = (ui8 *)target + cpyLen;

            if (len >= _bufferSize)
            {
                return ReadFromFile(target, len, read);
            }

            if (read) *read = cpyLen;
            _readBufferCurrentSize = 0;
            _bufferPos = 0;
            if (!ReadFromFile(_internalBuffer.get(), _bufferSize, &_readBufferCurrentSize))
            {
                return false;
            }
        }

        ui32 cpyLen = std::min(_readBufferCurrentSize - _bufferPos, len);
        readFromBuffer(target, cpyLen);
        if (read) *read += cpyLen;

        return true;
    }
#if ENABLE_FILE_STATS
    ++_stats.unbufferedReads;
#endif
    return ReadFromFile(target, len, read);
}

NOINLINE bool File::Write(const void *source, ui32 len, ui32 *written)
{
    ASSUME(IsOpened());
    ASSUME(source || len == 0);
    ASSUME(_procMode && FileProcMode::Write);
    ASSUME(_bufferPos <= _bufferSize);

    if (written) *written = 0;

    if (_bufferSize)
    {
    #if ENABLE_FILE_STATS
        ++_stats.bufferedWrites;
    #endif

        if (!CancelCachedRead())
        {
            return false;
        }

        if (_bufferSize - _bufferPos < len)
        {
            if (!File::Flush())
            {
                return false;
            }
            return WriteToFile(source, len, written);
        }

        memcpy(_internalBuffer.get() + _bufferPos, source, len);
        _bufferPos += len;
    #if ENABLE_FILE_STATS
        ++_stats.writesToBufferCount;
        _stats.bytesToBufferWritten += len;
    #endif

        if (written) *written = len;
        return true;
    }

#if ENABLE_FILE_STATS
    ++_stats.unbufferedWrites;
#endif
    return WriteToFile(source, len, written);
}

NOINLINE bool File::Flush()
{
    ASSUME(IsOpened());
    ASSUME(_bufferPos <= _bufferSize);
    FlushSystemCaches();
    if (_readBufferCurrentSize)
    {
        return true;
    }
    if (_bufferPos)
    {
        bool result = WriteToFile(_internalBuffer.get(), _bufferPos, 0);
        if (result)
        {
            _bufferPos = 0;
        }
        return result;
    }
    return true;
}

bool File::IsBufferingSupported() const
{
    return true;
}

NOINLINE bool File::BufferSet(ui32 size, bufferType &&buffer)
{
    ASSUME(size || buffer.get() == nullptr);

    if (buffer.get() == _internalBuffer.get() && buffer.get_deleter() == _internalBuffer.get_deleter() && size == _bufferSize)
    {
        return true;
    }
    if (!File::Flush() || !File::CancelCachedRead())
    {
        return false;
    }
    if (buffer)
    {
        _internalBuffer = std::move(buffer);
    }
    else
    {
        if (size == 0)
        {
            _internalBuffer.reset();
        }
        else
        {
            _internalBuffer = bufferType((ui8 *)malloc(size), [](ui8 *ptr) { free(ptr); });
        }
    }
    ASSUME(_bufferPos == 0);
    ASSUME(_readBufferCurrentSize == 0);
    _bufferSize = size;
    return true;
}

std::pair<ui32, const void *> File::BufferGet() const
{
    return {_bufferSize, _internalBuffer.get()};
}

bool File::IsSeekSupported() const
{
    return true;
}

Result<i64> File::OffsetGet(FileOffsetMode offsetMode)
{
    ASSUME(IsOpened());

    if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }

    if (offsetMode == FileOffsetMode::FromEnd)
    {
        if (!CancelCachedRead() || !File::Flush())
        {
            return DefaultError::UnknownError();
        }
    }

    auto currentOffsetResult = CurrentFileOffset();
    if (!currentOffsetResult)
    {
        return currentOffsetResult.GetError();
    }

    i64 offsetFromBegin = currentOffsetResult.Unwrap() - _offsetToStart;

    if (offsetMode == FileOffsetMode::FromBegin)
    {
        ASSUME(_bufferPos == 0 || _internalBuffer.get());
        if (_readBufferCurrentSize)
        {
            offsetFromBegin -= (i64)(_readBufferCurrentSize - _bufferPos);
        }
        else
        {
            offsetFromBegin += (i64)_bufferPos;
        }

        return offsetFromBegin;
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

FileProcMode File::ProcMode() const
{
    ASSUME(IsOpened());
    return _procMode;
}

FileCacheMode File::CacheMode() const
{
    ASSUME(IsOpened());
    return _cacheMode;
}