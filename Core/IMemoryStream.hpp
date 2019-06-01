#pragma once

#include "TypeIdentifiable.hpp"
#include "Allocators.hpp"
#include "DataHolder.hpp"

WARNING_PUSH
WARNING_DISABLE_INTEGRAL_CONSTANT_OVERFLOW

namespace StdLib
{
    struct NOVTABLE IMemoryStream
    {
        virtual ~IMemoryStream() = default;

        [[nodiscard]] virtual ui64 Size() const = 0;
        [[nodiscard]] virtual ui64 Resize(ui64 newSize) = 0; // returns final size, it can be lower than newSize if the stream had failed to allocate enough memory; memory address and size might change after this call
        virtual void Flush() = 0; // memory address and size might change after this call
        [[nodiscard]] virtual std::byte *Memory() = 0;
        [[nodiscard]] virtual const std::byte *Memory() const = 0;
        [[nodiscard]] virtual const std::byte *CMemory() const = 0;
        [[nodiscard]] virtual bool IsReadable() const = 0;
        [[nodiscard]] virtual bool IsWritable() const = 0;
		[[nodiscard]] virtual TypeId Type() const = 0;
    };

    // uses a fixed sized buffer
    template <uiw FixedSize> class MemoryStreamFixed final : public IMemoryStream, public TypeIdentifiable<MemoryStreamFixed<FixedSize>>
    {
		std::byte _buffer[FixedSize];
        uiw _currentSize = 0;

    public:
		using TypeIdentifiable<MemoryStreamFixed<FixedSize>>::GetTypeId;
		using TypeIdentifiable<MemoryStreamFixed<FixedSize>>::GetTypeName;

        MemoryStreamFixed() = default;

        [[nodiscard]] virtual ui64 Size() const override
        {
            return _currentSize;
        }

        [[nodiscard]] virtual ui64 Resize(ui64 newSize) override
        {
            if (newSize > FixedSize)
            {
                newSize = FixedSize;
            }
            _currentSize = (uiw)newSize;
            return _currentSize;
        }

        virtual void Flush() override
        {}

        [[nodiscard]] virtual std::byte *Memory() override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const std::byte *Memory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const std::byte *CMemory() const override
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

		[[nodiscard]] virtual TypeId Type() const override
		{
			return GetTypeId();
		}
    };

    // uses an externally provided buffer
    class MemoryStreamFixedExternal final : public IMemoryStream, public TypeIdentifiable<MemoryStreamFixedExternal>
    {
		std::byte *_writeBuffer = nullptr;
        const std::byte *_readBuffer = nullptr;
        uiw _maxSize = 0;
        uiw _currentSize = 0;

    public:
        MemoryStreamFixedExternal() = default;

        MemoryStreamFixedExternal(void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer(static_cast<std::byte *>(buffer)), _readBuffer(static_cast<const std::byte *>(buffer)), _maxSize(maxSize), _currentSize(currentSize)
        {}

        MemoryStreamFixedExternal(const void *buffer, uiw maxSize, uiw currentSize = 0) : _writeBuffer(nullptr), _readBuffer(static_cast<const std::byte *>(buffer)), _maxSize(maxSize), _currentSize(currentSize)
        {}

        void SetBuffer(void *buffer, uiw maxSize, uiw currentSize = 0)
        {
            this->_readBuffer = static_cast<const std::byte *>(buffer);
            this->_writeBuffer = static_cast<std::byte *>(buffer);
            this->_maxSize = maxSize;
            this->_currentSize = currentSize;
        }

        void SetBuffer(const void *buffer, uiw maxSize, uiw currentSize = 0)
        {
            this->_readBuffer = static_cast<const std::byte *>(buffer);
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
            _currentSize = static_cast<uiw>(newSize);
            return _currentSize;
        }

        virtual void Flush() override
        {}

        [[nodiscard]] virtual std::byte *Memory() override
        {
            return _writeBuffer;
        }

        [[nodiscard]] virtual const std::byte *Memory() const override
        {
            return _readBuffer;
        }

        [[nodiscard]] virtual const std::byte *CMemory() const override
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

		[[nodiscard]] virtual TypeId Type() const override
		{
			return GetTypeId();
		}
    };

    // uses an allocator
    template <typename AllocatorType = Allocator::Malloc> class MemoryStreamAllocator final : public IMemoryStream, public TypeIdentifiable<MemoryStreamAllocator<AllocatorType>>
    {
        AllocatorType _allocator{};
		std::byte *_buffer = nullptr;
        uiw _currentSize = 0;

    public:
		using TypeIdentifiable<MemoryStreamAllocator<AllocatorType>>::GetTypeId;
		using TypeIdentifiable<MemoryStreamAllocator<AllocatorType>>::GetTypeName;

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
                _currentSize = static_cast<uiw>(newSize);
                newSize += newSize == 0;
                _buffer = _allocator.Reallocate(_buffer, _currentSize);
            }
            return _currentSize;
        }

        virtual void Flush() override
        {}

        [[nodiscard]] virtual std::byte *Memory() override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const std::byte *Memory() const override
        {
            return _buffer;
        }

        [[nodiscard]] virtual const std::byte *CMemory() const override
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

		[[nodiscard]] virtual TypeId Type() const override
		{
			return GetTypeId();
		}
    };

    template <uiw LocalSize, uiw LocalAlignment = 8> class MemoryStreamFromDataHolder final : public IMemoryStream, public TypeIdentifiable<MemoryStreamFromDataHolder<LocalSize, LocalAlignment>>
    {
        using holderType = DataHolder<LocalSize, LocalAlignment>;

        holderType _data;
        const std::byte *(*_provide)(const holderType &data);
        void(*_flush)(holderType &data);
        void(*_destroy)(holderType &data);
        uiw _size;

    private:
        MemoryStreamFromDataHolder() = delete;
        MemoryStreamFromDataHolder(holderType &&data, uiw size, decltype(_provide) provide, decltype(_flush) flush, decltype(_destroy) destroy) : _data(std::move(data)), _size(size), _provide(provide), _flush(flush), _destroy(destroy) {}

    public:
		using TypeIdentifiable<MemoryStreamFromDataHolder<LocalSize, LocalAlignment>>::GetTypeId;
		using TypeIdentifiable<MemoryStreamFromDataHolder<LocalSize, LocalAlignment>>::GetTypeName;

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

        [[nodiscard]] virtual std::byte *Memory() override
        {
            SOFTBREAK;
            return nullptr;
        }

        [[nodiscard]] virtual const std::byte *Memory() const override
        {
            return _provide(_data);
        }

        [[nodiscard]] virtual const std::byte *CMemory() const override
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

		[[nodiscard]] virtual TypeId Type() const override
		{
			return GetTypeId();
		}

        template <typename T> [[nodiscard]] static MemoryStreamFromDataHolder New(holderType &&data, uiw size, decltype(_provide) provide = nullptr, decltype(_flush) flush = nullptr)
        {
            auto destroy = [](holderType &data)
            {
                data.template Destroy<T>();
            };
			if (!provide)
			{
				provide = [](const holderType &holder)
				{
					return &holder.Get<std::byte>();
				};
			}
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

WARNING_POP