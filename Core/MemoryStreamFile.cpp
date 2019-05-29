#include "_PreHeader.hpp"
#include "MemoryStreamFile.hpp"

using namespace StdLib;

MemoryStreamFile::MemoryStreamFile(IMemoryStream &stream, FileProcModes::FileProcMode procMode, ui64 offset, Error<> *error)
{
    Error<> dummyError = this->Open(stream, procMode, offset);
    if (error) *error = dummyError;
}

MemoryStreamFile::MemoryStreamFile(MemoryStreamFile &&source) noexcept
{
    this->_stream = source._stream;
    source._stream = 0;
    this->_offset = source._offset;
    this->_procMode = source._procMode;
    this->_startOffset = source._startOffset;
}

MemoryStreamFile &MemoryStreamFile::operator = (MemoryStreamFile &&source) noexcept
{
    ASSUME(this != &source);
    this->_stream = source._stream;
    source._stream = 0;
    this->_offset = source._offset;
    this->_procMode = source._procMode;
    this->_startOffset = source._startOffset;
    return *this;
}

Error<> MemoryStreamFile::Open(IMemoryStream &stream, FileProcModes::FileProcMode procMode, ui64 offset)
{
    _stream = 0;
    _offset = 0;
    _startOffset = 0;

    if (!procMode.Contains(FileProcModes::Read) && !procMode.Contains(FileProcModes::Write))
    {
        return DefaultError::InvalidArgument("Neither FileProcModes::Read, nor FileProcModes::Write is set");
    }

    if (procMode.Contains(FileProcModes::Read))
    {
        if (stream.IsReadable() == false)
        {
            return DefaultError::AccessDenied("Requested read proc mode, but the stream is not readable");
        }
    }
    if (procMode.Contains(FileProcModes::Write))
    {
        if (stream.IsWritable() == false)
        {
            return DefaultError::AccessDenied("Requested write proc mode, but the stream is not readable");
        }
    }

    _offset = _startOffset = std::min(offset, stream.Size());
    _stream = &stream;
    _procMode = procMode;

    return DefaultError::Ok();
}

TypeId MemoryStreamFile::Type() const
{
	return GetTypeId();
}

void MemoryStreamFile::Close()
{
    _stream = 0;
}

bool MemoryStreamFile::IsOpen() const
{
    return _stream != 0;
}

bool MemoryStreamFile::Read(void *RSTR target, ui32 len, ui32 *RSTR read)
{
    ASSUME(_stream && _procMode.Contains(FileProcModes::Read));
    ASSUME(len == 0 || target);
    ui64 diff = _offset <= _stream->Size() ? _stream->Size() - _offset : 0;
    len = std::min(len, (ui32)diff);
    if (_offset + len < _offset)  //  overflow
    {
        len = (ui32)(uiw_max - _offset);
    }
    MemOps::Copy((ui8 *)target, _stream->Memory() + _offset, len);
    _offset += len;
    if (read) *read = len;
    return true;
}

bool MemoryStreamFile::Write(const void *source, ui32 len, ui32 *RSTR written)
{
    ASSUME(_stream && _procMode.Contains(FileProcModes::Write));
    ASSUME(len == 0 || source);
    ui64 writeEnd = _offset + len;
    if (writeEnd < _offset)  //  overflow
    {
        writeEnd = uiw_max;
        len = (ui32)(uiw_max - _offset);
    }
    if (writeEnd > _stream->Size())
    {
        ui64 newSize = _stream->Resize(writeEnd);
        if (newSize <= _offset)
        {
            len = 0;
        }
        else
        {
            len = (ui32)(newSize - _offset);
        }
    }
    MemOps::Copy(_stream->Memory() + _offset, (ui8 *)source, len);
    _offset += len;
    if (written) *written = len;
    return true;
}

bool MemoryStreamFile::Flush()
{
    ASSUME(_stream);
    _stream->Flush();
    return true;
}

bool MemoryStreamFile::IsBufferingSupported() const
{
    return false;
}

bool MemoryStreamFile::Buffer(ui32 size, bufferType &&buffer)
{
    return false;
}

std::pair<ui32, const void *> MemoryStreamFile::Buffer() const
{
    return {0, nullptr};
}

bool MemoryStreamFile::IsSeekSupported() const
{
    return true;
}

Result<i64> MemoryStreamFile::Offset(FileOffsetMode offsetMode)
{
    ASSUME(_stream);
    if (offsetMode == FileOffsetMode::FromBegin)
    {
        return (i64)_offset - (i64)_startOffset;
    }
    else if (offsetMode == FileOffsetMode::FromCurrent)
    {
        return 0;
    }
    else
    {
        ASSUME(offsetMode == FileOffsetMode::FromEnd);
        return (i64)_offset - (i64)_stream->Size();
    }
}

Result<i64> MemoryStreamFile::Offset(FileOffsetMode offsetMode, i64 offset)
{
    ASSUME(_stream);
    ASSUME(_startOffset <= _stream->Size());

    if (offset < 0) // backward
    {
        if (offsetMode == FileOffsetMode::FromBegin)
        {
            SOFTBREAK;
            return DefaultError::InvalidArgument("Cannot specify negative offset with FileOffsetMode::FromBegin");
        }
        else if (offsetMode == FileOffsetMode::FromCurrent)
        {
            if (_offset + offset < _startOffset) // underflow
            {
                SOFTBREAK;
                return DefaultError::InvalidArgument("Specified offset is out of range for the file");
            }
            else
            {
                _offset += offset;
            }
        }
        else
        {
            ASSUME(offsetMode == FileOffsetMode::FromEnd);

            if (_stream->Size() + offset < _startOffset) // underflow
            {
                SOFTBREAK;
                return DefaultError::InvalidArgument("Specified offset is out of range for the file");
            }
            else
            {
                _offset = _stream->Size() + offset;
            }
        }
    }
    else // forward
    {
        if (offsetMode == FileOffsetMode::FromBegin)
        {
            if (offset > (i64)(_stream->Size() - _startOffset)) // overflow
            {
                SOFTBREAK;
                return DefaultError::InvalidArgument("Specified offset is out of range for the file");
            }
            else
            {
                _offset = _startOffset + offset;
            }
        }
        else if (offsetMode == FileOffsetMode::FromCurrent)
        {
            if (offset > (i64)(_stream->Size() > _startOffset)) // overflow
            {
                SOFTBREAK;
                return DefaultError::InvalidArgument("Specified offset is out of range for the file");
            }
            else
            {
                _offset += offset;
            }
        }
        else
        {
            ASSUME(offsetMode == FileOffsetMode::FromEnd);

            if (offset)
            {
                SOFTBREAK;
                return DefaultError::InvalidArgument("Cannot specify positive offset with FileOffsetMode::FromEnd");
            }
        }
    }

    return _offset - _startOffset;
}

Result<ui64> MemoryStreamFile::Size()
{
    ASSUME(_stream);
    return _stream->Size() - _startOffset;
}

Error<> MemoryStreamFile::Size(ui64 newSize)
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

FileProcModes::FileProcMode MemoryStreamFile::ProcMode() const
{
    ASSUME(_stream);
    return _procMode;
}

FileCacheModes::FileCacheMode MemoryStreamFile::CacheMode() const
{
    ASSUME(_stream);
    return FileCacheModes::Default;
}