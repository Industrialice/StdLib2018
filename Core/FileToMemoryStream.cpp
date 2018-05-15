#include "FileToMemoryStream.hpp"

using namespace StdLib;

static bool IsProcModeSet(FileProcMode procModeCombo, FileProcMode flag);

FileToMemoryStream::FileToMemoryStream(IMemoryStream &stream, FileProcMode procMode, Error<> *error)
{
    Error<> dummyError = this->Open(stream, procMode);
    if (error) *error = dummyError;
}

FileToMemoryStream::FileToMemoryStream(FileToMemoryStream &&source)
{
    this->_stream = source._stream;
    source._stream = 0;
    this->_offset = source._offset;
    this->_procMode = source._procMode;
    this->_startOffset = source._startOffset;
}

FileToMemoryStream &FileToMemoryStream::operator = (FileToMemoryStream &&source)
{
    ASSUME(this != &source);
    this->_stream = source._stream;
    source._stream = 0;
    this->_offset = source._offset;
    this->_procMode = source._procMode;
    this->_startOffset = source._startOffset;
    return *this;
}

Error<> FileToMemoryStream::Open(IMemoryStream &stream, FileProcMode procMode)
{
    _stream = 0;
    _offset = 0;
    _startOffset = 0;

    if (!IsProcModeSet(procMode, FileProcMode::Read) && !IsProcModeSet(procMode, FileProcMode::Write))
    {
        return DefaultError::InvalidArgument("Neither FileProcMode::Read, nor FileProcMode::Write is set");
    }

    if (IsProcModeSet(procMode, FileProcMode::Read))
    {
        if (stream.IsReadable() == false)
        {
            return DefaultError::AccessDenied("Requested read proc mode, but the stream is not readable");
        }
    }
    if (IsProcModeSet(procMode, FileProcMode::Write))
    {
        if (stream.IsWritable() == false)
        {
            return DefaultError::AccessDenied("Requested write proc mode, but the stream is not readable");
        }
    }
    if (IsProcModeSet(procMode, FileProcMode::WriteAppend))
    {
        _offset = _startOffset = stream.Size();
    }

    _stream = &stream;
    _procMode = procMode;

    return DefaultError::Ok();
}

void FileToMemoryStream::Close()
{
    _stream = 0;
}

bool FileToMemoryStream::IsOpened() const
{
    return _stream != 0;
}

bool FileToMemoryStream::Read(void *target, ui32 len, ui32 *read)
{
    ASSUME(_stream && IsProcModeSet(_procMode, FileProcMode::Read));
    ASSUME(len == 0 || target);
    uiw diff = _offset <= _stream->Size() ? _stream->Size() - _offset : 0;
    len = std::min<uiw>(len, diff);
    if (_offset + len < _offset)  //  overflow
    {
        len = uiw_max - _offset;
    }
    memcpy(target, _stream->Memory() + _offset, len);
    _offset += len;
    if (read) *read = len;
    return true;
}

bool FileToMemoryStream::Write(const void *source, ui32 len, ui32 *written)
{
    ASSUME(_stream && IsProcModeSet(_procMode, FileProcMode::Write));
    ASSUME(len == 0 || source);
    uiw writeEnd = _offset + len;
    if (writeEnd < _offset)  //  overflow
    {
        writeEnd = uiw_max;
        len = uiw_max - _offset;
    }
    if (writeEnd > _stream->Size())
    {
        uiw newSize = _stream->Resize(writeEnd);
        if (newSize <= _offset)
        {
            len = 0;
        }
        else
        {
            len = newSize - _offset;
        }
    }
    memcpy(_stream->Memory() + _offset, source, len);
    _offset += len;
    if (written) *written = len;
    return true;
}

bool FileToMemoryStream::Flush()
{
    ASSUME(_stream);
    return true;
}

bool FileToMemoryStream::IsBufferingSupported() const
{
    return false;
}

bool FileToMemoryStream::BufferSet(ui32 size, bufferType &&buffer)
{
    return false;
}

std::pair<ui32, const void *> FileToMemoryStream::BufferGet() const
{
    return {0, nullptr};
}

bool FileToMemoryStream::IsSeekSupported() const
{
    return true;
}

Result<i64> FileToMemoryStream::OffsetGet(FileOffsetMode offsetMode)
{
    ASSUME(_stream);
    if (offsetMode == FileOffsetMode::FromBegin)
    {
        return _offset - _startOffset;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }
    else  //  if( offsetMode == FileOffsetMode::FromEnd )
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        return _offset - _stream->Size();
    }
}

Result<i64> FileToMemoryStream::OffsetSet(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(_stream);

    if (offset <= 0)  //  backward
    {
        if (offsetMode == FileOffsetMode::FromBegin)
        {
            _offset = _startOffset;
        }
        else if (offsetMode == FileOffsetMode::FromCurrent)
        {
            if (_offset + offset < _startOffset)  //  underflow
            {
                _offset = _startOffset;
            }
            else
            {
                _offset += offset;
            }
        }
        else  //  if( offsetMode == FileOffsetMode::FromEnd )
        {
            ASSUME(offsetMode == FileOffsetMode::FromEnd);

            if (_stream->Size() + offset < _startOffset)  //  underflow
            {
                _offset = _startOffset;
            }
            else
            {
                _offset = _stream->Size() + offset;
            }
        }
    }
    else  //  forward
    {
        if (offsetMode == FileOffsetMode::FromBegin)
        {
            uiw diff = uiw_max - _startOffset;
            if (offset >= (i64)diff)  //  overflow
            {
                _offset = (i64)uiw_max;
            }
            else
            {
                _offset = _startOffset + offset;
            }
        }
        else if (offsetMode == FileOffsetMode::FromCurrent)
        {
            uiw diffToOverflow = uiw_max - _offset;
            if (offset > (i64)diffToOverflow)  //  overflow
            {
                _offset = (i64)uiw_max;
            }
            else
            {
                _offset += offset;
            }
        }
        else  //  if( offsetMode == FileOffsetMode::FromEnd )
        {
            ASSUME(offsetMode == FileOffsetMode::FromEnd);

            uiw diffToOverflow = uiw_max - _stream->Size();
            if (offset >= (i64)diffToOverflow)  //  overflow
            {
                _offset = (i64)uiw_max;
            }
            else
            {
                _offset = _stream->Size() + offset;
            }
        }
    }

    return _offset - _startOffset;
}

Result<ui64> FileToMemoryStream::SizeGet() const
{
    ASSUME(_stream);
    return _stream->Size() - _startOffset;
}

Error<> FileToMemoryStream::SizeSet(ui64 newSize)
{
    ASSUME(_stream);
    newSize += _startOffset;
    if (newSize > _startOffset)
    {
        if (_stream->Resize(newSize) == newSize)
        {
            return DefaultError::Ok();
        }
    }
    return DefaultError::OutOfMemory();
}

FileProcMode FileToMemoryStream::ProcMode() const
{
    ASSUME(_stream);
    return _procMode;
}

FileCacheMode FileToMemoryStream::CacheMode() const
{
    ASSUME(_stream);
    return FileCacheMode::Default;
}

bool IsProcModeSet(FileProcMode procModeCombo, FileProcMode flag)
{
    return procModeCombo == (procModeCombo + flag);
}