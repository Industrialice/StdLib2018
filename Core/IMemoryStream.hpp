#pragma once

#include "CoreHeader.hpp"
#include "Allocators.hpp"

namespace StdLib
{
    struct IMemoryStream
    {
        virtual ~IMemoryStream() = default;

        [[nodiscard]] virtual uiw Size() const = 0;
        [[nodiscard]] virtual uiw Resize(uiw newSize) = 0; // returns final size, it can be lower than newSize if the stream had failed to allocate enough memory
        [[nodiscard]] virtual ui8 *Memory() = 0;
        [[nodiscard]] virtual const ui8 *Memory() const = 0;
        [[nodiscard]] virtual const ui8 *CMemory() const = 0;
        [[nodiscard]] virtual bool IsReadable() const = 0;
        [[nodiscard]] virtual bool IsWritable() const = 0;
    };

    // uses a fixed sized buffer
    template <uiw size> class MemoryStreamFixed final : public IMemoryStream
    {
        ui8 _buffer[size];
        uiw _currentSize = 0;

    public:
        MemoryStreamFixed() = default;

        [[nodiscard]] virtual uiw Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual uiw Resize(uiw newSize) override
        {
            if (newSize > size)
            {
                newSize = size;
            }
            _currentSize = newSize;
            return _currentSize;
        }

        [[nodiscard]] virtual ui8 *Memory() override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const ui8 *Memory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const ui8 *CMemory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual bool IsReadable() const override
        {
            return true;
        }

        [[nodiscard]] virtual bool IsWritable() const override
        {
            return true;
        }
    };

    // uses an externally provided buffer
    class MemoryStreamFixedExt final : public IMemoryStream
    {
        ui8 *_writeBuffer = nullptr;
        const ui8 *_readBuffer = nullptr;
        uiw _maxSize = 0;
        uiw _currentSize = 0;

    public:
        MemoryStreamFixedExt() = default;

        MemoryStreamFixedExt(void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer((ui8 *)buffer), _readBuffer((ui8 *)buffer), _maxSize(maxSize), _currentSize(currentSize)
        {}

        MemoryStreamFixedExt(const void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer(nullptr), _readBuffer((ui8 *)buffer), _maxSize(maxSize), _currentSize(currentSize)
        {}

        void SetBuffer(void *buffer, uiw maxSize, uiw currentSize = 0)
        {
            this->_readBuffer = (const ui8 *)buffer;
            this->_writeBuffer = (ui8 *)buffer;
            this->_maxSize = maxSize;
            this->_currentSize = currentSize;
        }

        void SetBuffer(const void *buffer, uiw maxSize, uiw currentSize = 0)
        {
            this->_readBuffer = (const ui8 *)buffer;
            this->_writeBuffer = nullptr;
            this->_maxSize = maxSize;
            this->_currentSize = currentSize;
        }

        [[nodiscard]] virtual uiw Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual uiw Resize(uiw newSize) override
        {
            if (newSize > _maxSize)
            {
                newSize = _maxSize;
            }
            _currentSize = newSize;
            return _currentSize;
        }

        [[nodiscard]] virtual ui8 *Memory() override
        {
            return _writeBuffer;
        }

        [[nodiscard]] virtual const ui8 *Memory() const override
        {
            return _readBuffer;
        }

        [[nodiscard]] virtual const ui8 *CMemory() const override
        {
            return _readBuffer;
        }

        [[nodiscard]] virtual bool IsReadable() const override
        {
            return _readBuffer != 0;
        }

        [[nodiscard]] virtual bool IsWritable() const override
        {
            return _writeBuffer != 0;
        }
    };

    // uses an allocator
    template <typename AllocatorType = Allocator::MallocBased> class MemoryStreamAllocator final : public IMemoryStream
    {
        AllocatorType _allocator{};
        ui8 *_buffer = nullptr;
        uiw _currentSize = 0;

    public:
        ~MemoryStreamAllocator()
        {
            _allocator.Free(_buffer);
        }

        MemoryStreamAllocator() = default;

        [[nodiscard]] virtual uiw Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual uiw Resize(uiw newSize) override
        {
            if (newSize != _currentSize)
            {
                _currentSize = newSize;
                newSize += newSize == 0;
                _buffer = _allocator.Reallocate(_buffer, newSize);
            }
            return _currentSize;
        }

        [[nodiscard]] virtual ui8 *Memory() override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const ui8 *Memory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const ui8 *CMemory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual bool IsReadable() const override
        {
            return true;
        }

        [[nodiscard]] virtual bool IsWritable() const override
        {
            return true;
        }
    };

    // TODO: add a fixed buffered mem stream that uses a heap if the fixed buffer is not enough
    // TODO: also add heap with reserve
}