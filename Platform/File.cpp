#include "_PreHeader.hpp"
#include "File.hpp"

using namespace StdLib;

File::~File()
{
    Close();
}

File::File(const FilePath &pnn, FileOpenMode openMode, FileProcModes::FileProcMode procMode, ui64 offset, FileCacheModes::FileCacheMode cacheMode, FileShareModes::FileShareMode shareMode, Error<> *error)
{
    Error<> tempError = File::Open(pnn, openMode, procMode, offset, cacheMode, shareMode);
    if (error) *error = tempError;
}

File::File(FileHandle osFileDescriptor, bool isGettingFileDescriptorOwnership, ui64 offset, Error<> *error)
{
    Error<> tempError = File::Open(osFileDescriptor, isGettingFileDescriptorOwnership, offset);
    if (error) *error = tempError;
}

File::File(File &&other) noexcept
{
    _handle = other._handle;
    other._handle = FileHandle_undefined;
    _internalBuffer = std::move(other._internalBuffer);
    _bufferSize = other._bufferSize;
    _bufferPos = other._bufferPos;
    _readBufferCurrentSize = other._readBufferCurrentSize;
    _offsetToStart = other._offsetToStart;
    _isOwningFileHandle = other._isOwningFileHandle;
    _openMode = other._openMode;
    _procMode = other._procMode;
    _cacheMode = other._cacheMode;
#ifdef STDLIB_ENABLE_FILE_STATS
    _stats = other._stats;
#endif
#ifdef PLATFORM_WINXP
    _pnn = std::move(other._pnn);
#endif
}

File &File::operator = (File &&other) noexcept
{
    File::Close();

    _handle = other._handle;
    other._handle = FileHandle_undefined;
    _internalBuffer = std::move(other._internalBuffer);
    _bufferSize = other._bufferSize;
    _bufferPos = other._bufferPos;
    _readBufferCurrentSize = other._readBufferCurrentSize;
    _offsetToStart = other._offsetToStart;
    _isOwningFileHandle = other._isOwningFileHandle;
    _openMode = other._openMode;
    _procMode = other._procMode;
    _cacheMode = other._cacheMode;
#ifdef STDLIB_ENABLE_FILE_STATS
    _stats = other._stats;
#endif
#ifdef PLATFORM_WINXP
    _pnn = std::move(other._pnn);
#endif

    return *this;
}

#ifdef STDLIB_ENABLE_FILE_STATS
auto File::StatsGet() const -> FileStats
{
    ASSUME(IsOpen());
    return _stats;
}

void File::StatsReset()
{
    ASSUME(IsOpen());
    _stats = {};
}
#endif

FileOpenMode File::OpenMode() const
{
    ASSUME(IsOpen());
    return _openMode;
}

FileHandle File::OsFileDescriptor() const
{
    ASSUME(IsOpen());
    return _handle;
}

FileHandle File::CloseAndGetOsFileDescriptor()
{
    ASSUME(IsOpen());
    _isOwningFileHandle = false;
    FileHandle handle = _handle;
    File::Close();
    return handle;
}

bool File::FlushInternal()
{
    return FlushWriteBuffers(false);
}

TypeId File::Type() const
{
	return GetTypeId();
}

bool File::IsOpen() const
{
    return _handle != FileHandle_undefined;
}

NOINLINE bool File::Read(void *RSTR target, ui32 len, ui32 *RSTR read)
{
    ASSUME(IsOpen());
    ASSUME(target || len == 0);
    ASSUME(_procMode.Contains(FileProcModes::Read));

    auto readFromBuffer = [this](void *target, ui32 len)
    {
        MemOps::Copy(static_cast<std::byte *>(target), _internalBuffer.get() + _bufferPos, len);
        _bufferPos += len;
    #ifdef STDLIB_ENABLE_FILE_STATS
        ++_stats.readsFromBufferCount;
        _stats.bytesFromBufferRead += len;
    #endif
    };

	if (read) *read = 0;

    if (!FlushWriteBuffers(false)) // flush any pending writes
    {
        return false;
    }

    ASSUME(_bufferPos <= _readBufferCurrentSize);

    if (_readBufferCurrentSize - _bufferPos <= len) // we don't have enough info in the read buffer to fulfill the request
    {
        ui32 cpyLen = _readBufferCurrentSize - _bufferPos;
        readFromBuffer(target, cpyLen);
        len -= cpyLen;
        target = static_cast<std::byte *>(target) + cpyLen;

        if (len >= _bufferSize)
        {
			#ifdef STDLIB_ENABLE_FILE_STATS
				++_stats.unbufferedReads;
			#endif
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

	#ifdef STDLIB_ENABLE_FILE_STATS
		++_stats.bufferedReads;
	#endif

    return true;
}

NOINLINE bool File::Write(const void *RSTR source, ui32 len, ui32 *RSTR written)
{
    ASSUME(IsOpen());
    ASSUME(source || len == 0);
    ASSUME(_procMode.Contains(FileProcModes::Write));
    ASSUME(_bufferPos <= _bufferSize);

    if (written) *written = 0;

    if (!CancelCachedRead()) // if there's any cached read data, we need to flush it and restore the file pointer
    {
        return false;
    }

    if (_bufferSize - _bufferPos < len) // not enough space in the buffer to hold the new data
    {
        if (!FlushWriteBuffers(false)) // write the current contents of the buffer
        {
            return false;
        }
		#ifdef STDLIB_ENABLE_FILE_STATS
			++_stats.unbufferedWrites;
		#endif
        return WriteToFile(source, len, written);
    }

	#ifdef STDLIB_ENABLE_FILE_STATS
		++_stats.bufferedWrites;
	#endif

    MemOps::Copy(_internalBuffer.get() + _bufferPos, static_cast<const std::byte *>(source), len);
    _bufferPos += len;
	#ifdef STDLIB_ENABLE_FILE_STATS
		++_stats.writesToBufferCount;
		_stats.bytesToBufferWritten += len;
	#endif

    if (written) *written = len;
    return true;
}

bool File::Flush()
{
    return CancelCachedRead() && FlushWriteBuffers(true);
}

bool File::IsBufferingSupported() const
{
    return true;
}

NOINLINE bool File::Buffer(ui32 size, bufferType &&buffer)
{
	ASSUME(IsOpen());
    ASSUME(size || buffer.get() == nullptr);

    if (buffer.get() == _internalBuffer.get() && buffer.get_deleter() == _internalBuffer.get_deleter() && size == _bufferSize)
    {
        return true;
    }
	if (!FlushWriteBuffers(false) || !CancelCachedRead())
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
            _internalBuffer = bufferType(Allocator::Malloc::Allocate(size), [](std::byte *ptr) { free(ptr); });
        }
    }
    ASSUME(_bufferPos == 0);
    ASSUME(_readBufferCurrentSize == 0);
    _bufferSize = size;
    return true;
}

std::pair<ui32, const void *> File::Buffer() const
{
	ASSUME(IsOpen());
    return {_bufferSize, _internalBuffer.get()};
}

bool File::IsSeekSupported() const
{
    return true;
}

Result<i64> File::Offset(FileOffsetMode offsetMode)
{
    ASSUME(IsOpen());

    if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }

    if (offsetMode == FileOffsetMode::FromEnd)
    {
        if (!CancelCachedRead() || !FlushWriteBuffers(false))
        {
            return DefaultError::UnknownError("Flushing buffers failed");
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
            offsetFromBegin -= static_cast<i64>(_readBufferCurrentSize - _bufferPos);
        }
        else
        {
            offsetFromBegin += static_cast<i64>(_bufferPos);
        }

        return offsetFromBegin;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);

        auto fileSize = File::Size();
        if (!fileSize)
        {
            return fileSize.GetError();
        }

        return offsetFromBegin - fileSize.Unwrap();
    }
}

FileProcModes::FileProcMode File::ProcMode() const
{
    ASSUME(IsOpen());
    return _procMode;
}

FileCacheModes::FileCacheMode File::CacheMode() const
{
    ASSUME(IsOpen());
    return _cacheMode;
}

bool File::FlushWriteBuffers(bool isFlushSystemCaches)
{
    ASSUME(IsOpen());
    ASSUME(_bufferPos <= _bufferSize);

    if (isFlushSystemCaches)
    {
        FlushSystemCaches();
    }

    if (_readBufferCurrentSize) // the buffer is currently used for reading, nothing to flush
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
