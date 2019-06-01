#pragma once

#include "GenericFuncs.hpp"

namespace StdLib
{
	// TODO: using template parameters can be made movable, copyable and RAII-destructible
    template <uiw Size, uiw Alignment = 8> class DataHolder
    {
		static_assert(Funcs::IsPowerOf2(Alignment), "alignment is not power of 2");

	public:
        static constexpr uiw size = (Size + (Alignment - 1)) & ~(Alignment - 1);
		static constexpr uiw alignment = Alignment;

	private:
        union
        {
            void *_address;
            alignas(Alignment) std::byte _local[size];
        };
        void (*_onMoving)(DataHolder &target, DataHolder &source);
    #ifdef DEBUG
        bool _isDestroyed = false;
        bool _isMovedFrom = false;
    #endif

    public:
    #ifdef DEBUG
        ~DataHolder()
        {
            ASSUME(_isDestroyed || _isMovedFrom);
        }
    #endif

        DataHolder() = delete;

        template <typename T> DataHolder(T &&source) noexcept
        {
            using clearT = std::remove_reference_t<T>;

            static_assert(alignof(clearT) <= Alignment, "Cannot satisfy alignment requirement with provided type");

            if constexpr (sizeof(clearT) <= size)
            {
                new (_local) clearT(std::forward<T>(source));
                _onMoving = [](DataHolder &target, DataHolder &sourceHolder)
                {
					ASSUME_DEBUG_ONLY(!sourceHolder._isDestroyed);
					ASSUME_DEBUG_ONLY(!sourceHolder._isMovedFrom);
                    new (target._local) clearT(std::move(sourceHolder.Get<clearT>()));
                    reinterpret_cast<clearT *>(sourceHolder._local)->~clearT();
                    target._onMoving = sourceHolder._onMoving;
                #ifdef DEBUG
                    target._isDestroyed = false;
                    target._isMovedFrom = false;
                    sourceHolder._isMovedFrom = true;
                #endif
                };
            }
            else
            {
                _address = new clearT(std::forward<T>(source));
                _onMoving = [](DataHolder &target, DataHolder &sourceHolder)
                {
                    ASSUME_DEBUG_ONLY(!sourceHolder._isDestroyed);
					ASSUME_DEBUG_ONLY(!sourceHolder._isMovedFrom);
                    target._address = sourceHolder._address;
                    sourceHolder._address = nullptr;
                    target._onMoving = sourceHolder._onMoving;
                #ifdef DEBUG
                    target._isDestroyed = false;
                    target._isMovedFrom = false;
                    sourceHolder._isMovedFrom = true;
                #endif
                };
            }
        }

        DataHolder(DataHolder &&source) noexcept
        {
            source._onMoving(*this, source);
        }

        DataHolder &operator = (DataHolder &&source) noexcept
        {
            source._onMoving(*this, source);
            return *this;
        }

        template <typename T> void Destroy() // do not call it from the constructor
        {
        #ifdef DEBUG
            ASSUME(!_isDestroyed && !_isMovedFrom);
            _isDestroyed = true;
        #endif
            if constexpr (sizeof(T) <= size)
            {
				reinterpret_cast<T *>(_local)->~T();
            }
            else
            {
                delete static_cast<T *>(_address);
            }
        }

        template <typename T> [[nodiscard]] T &Get()
        {
            ASSUME_DEBUG_ONLY(!_isDestroyed && !_isMovedFrom);
            if constexpr (sizeof(T) <= size)
            {
                return *reinterpret_cast<T *>(_local);
            }
            else
            {
                return *static_cast<T *>(_address);
            }
        }

        template <typename T> [[nodiscard]] const T &Get() const
        {
            ASSUME_DEBUG_ONLY(!_isDestroyed && !_isMovedFrom);
			if constexpr (sizeof(T) <= size)
			{
				return *reinterpret_cast<const T *>(_local);
			}
			else
			{
				return *static_cast<const T *>(_address);
			}
        }

        template <typename T> [[nodiscard]] constexpr bool IsPlacedLocally() const
        {
            return sizeof(T) <= size;
        }
    };
}