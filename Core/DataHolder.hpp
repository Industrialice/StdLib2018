#pragma once

namespace StdLib
{
    template <uiw Size> class DataHolder
    {
        static constexpr uiw size = (Size & 15) ? ((Size & ~15) + 16) : Size;
        union
        {
            void *_address;
            alignas(16) ui8 _local[size];
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

        template <typename T> DataHolder(T &&source)
        {
            using clearT = std::remove_reference_t<T>;

            static_assert(alignof(clearT) <= 16, "Objects with pecular alignment requirements are not allowed");

            if constexpr (sizeof(clearT) <= size)
            {
                new (_local) clearT(std::forward<T>(source));
                _onMoving = [](DataHolder &target, DataHolder &source)
                {
                    new (target._local) clearT(std::move(source.Get<clearT>()));
                    ((clearT *)source._local)->~clearT();
                    target._onMoving = source._onMoving;
                #ifdef DEBUG
                    source._isMovedFrom = true;
                #endif
                };
            }
            else
            {
                _address = new clearT(std::forward<T>(source));
                _onMoving = [](DataHolder &target, DataHolder &source)
                {
                    target._address = source._address;
                    source._address = nullptr;
                    target._onMoving = source._onMoving;
                #ifdef DEBUG
                    source._isMovedFrom = true;
                #endif
                };
            }
        }

        DataHolder(DataHolder &&source)
        {
            source._onMoving(*this, source);
        }

        DataHolder &operator = (DataHolder &&source)
        {
            source._onMoving(*this, source);
            return *this;
        }

        template <typename T> void Destroy() // do not call it on hold
        {
        #ifdef DEBUG
            ASSUME(!_isDestroyed && !_isMovedFrom);
            _isDestroyed = true;
        #endif
            if constexpr (sizeof(T) <= size)
            {
                ((T *)_local)->~T();
            }
            else
            {
                T *casted = (T *)_address;
                delete casted;
            }
        }

        template <typename T> T &Get()
        {
        #ifdef DEBUG
            ASSUME(!_isDestroyed && !_isMovedFrom);
        #endif
            if constexpr (sizeof(T) <= size)
            {
                return *(T *)_local;
            }
            else
            {
                return *(T *)_address;
            }
        }

        template <typename T> const T &Get() const
        {
        #ifdef DEBUG
            ASSUME(!_isDestroyed && !_isMovedFrom);
        #endif
            if constexpr (sizeof(T) <= size)
            {
                return *(T *)_local;
            }
            else
            {
                return *(T *)_address;
            }
        }

        template <typename T> constexpr bool IsPlacedLocally() const
        {
            return sizeof(T) <= size;
        }
    };
}