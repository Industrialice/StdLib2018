#pragma once

#include "CoreHeader.hpp"
#include "Allocators.hpp"
#include "DataHolder.hpp"

namespace StdLib
{
    struct NOVTABLE IMemoryStream
    {
        virtual ~IMemoryStream() = default;

        [[nodiscard]] virtual ui64 Size() const = 0;
        [[nodiscard]] virtual ui64 Resize(ui64 newSize) = 0; // returns final size, it can be lower than newSize if the stream had failed to allocate enough memory; memory address and size might change after this call
        virtual void Flush() = 0; // memory address and size might change after this call
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

        [[nodiscard]] virtual ui64 Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual ui64 Resize(ui64 newSize) override
        {
            if (newSize > size)
            {
                newSize = size;
            }
            _currentSize = (uiw)newSize;
            return _currentSize;
        }

        virtual void Flush() override
        {}

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
    class MemoryStreamFixedExternal final : public IMemoryStream
    {
        ui8 *_writeBuffer = nullptr;
        const ui8 *_readBuffer = nullptr;
        uiw _maxSize = 0;
        uiw _currentSize = 0;

    public:
        MemoryStreamFixedExternal() = default;

        MemoryStreamFixedExternal(void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer((ui8 *)buffer), _readBuffer((ui8 *)buffer), _maxSize(maxSize), _currentSize(currentSize)
        {}

        MemoryStreamFixedExternal(const void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer(nullptr), _readBuffer((ui8 *)buffer), _maxSize(maxSize), _currentSize(currentSize)
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

        [[nodiscard]] virtual ui64 Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual ui64 Resize(ui64 newSize) override
        {
            ASSUME(newSize <= uiw_max);
            if (newSize > _maxSize)
            {
                newSize = _maxSize;
            }
            _currentSize = (uiw)newSize;
            return _currentSize;
        }

        virtual void Flush() override
        {}

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

        [[nodiscard]] virtual ui64 Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual ui64 Resize(ui64 newSize) override
        {
            ASSUME(newSize <= uiw_max);
            if (newSize != _currentSize)
            {
                _currentSize = (uiw)newSize;
                newSize += newSize == 0;
                _buffer = _allocator.Reallocate(_buffer, (uiw)newSize);
            }
            return _currentSize;
        }

        virtual void Flush() override
        {}

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

    template <uiw LocalSize, uiw LocalAlignment = 8> class MemoryStreamFromDataHolder final : public IMemoryStream
    {
        using holderType = DataHolder<LocalSize, LocalAlignment>;

        holderType _data;
        const ui8 *(*_provide)(const holderType &data);
        void(*_flush)(holderType &data);
        void(*_destroy)(holderType &data);
        uiw _size;

    private:
        MemoryStreamFromDataHolder() = delete;
        MemoryStreamFromDataHolder(holderType &&data, uiw size, decltype(_provide) provide, decltype(_flush) flush, decltype(_destroy) destroy) : _data(std::move(data)), _size(size), _provide(provide), _flush(flush), _destroy(destroy) {}

    public:
        static constexpr uiw localSize = LocalSize;
		static constexpr uiw localAlignment = LocalAlignment;

        virtual ~MemoryStreamFromDataHolder() override
        {
            if (_destroy)
            {
                _destroy(_data);
            }
        }

        MemoryStreamFromDataHolder(MemoryStreamFromDataHolder &&other) noexcept : _data(std::move(other._data)), _size(other._size), _provide(other._provide), _destroy(other._destroy)
        {
            other._destroy = nullptr;
        }

        MemoryStreamFromDataHolder &operator = (MemoryStreamFromDataHolder &&other) noexcept
        {
            _data = std::move(other._data);
            _size = other._size;
            _provide = other._provide;
            _destroy = other._destroy;
            other._destroy = nullptr;
            return *this;
        }

        [[nodiscard]] virtual ui64 Size() const override
        {
            return _size;
        }

        [[nodiscard]] virtual ui64 Resize(ui64 newSize) override
        {
            return _size;
        }

        virtual void Flush() override
        {
            _flush(_data);
        }

        [[nodiscard]] virtual ui8 *Memory() override
        {
            SOFTBREAK;
            return nullptr;
        }

        [[nodiscard]] virtual const ui8 *Memory() const override
        {
            return _provide(_data);
        }

        [[nodiscard]] virtual const ui8 *CMemory() const override
        {
            return _provide(_data);
        }

        [[nodiscard]] virtual bool IsReadable() const override
        {
            return true;
        }

        [[nodiscard]] virtual bool IsWritable() const override
        {
            return false;
        }

        template <typename T> static MemoryStreamFromDataHolder New(holderType &&data, uiw size, decltype(_provide) provide, decltype(_flush) flush = nullptr)
        {
            auto destroy = [](holderType &data)
            {
                data.template Destroy<T>();
            };
            if (!flush)
            {
                flush = [](holderType &) {};
            }
            return MemoryStreamFromDataHolder(std::move(data), size, provide, flush, destroy);
        }
    };

    // TODO: add a fixed buffered mem stream that uses a heap if the fixed buffer is not enough
    // TODO: also add heap with reserve
}