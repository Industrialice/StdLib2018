#pragma once

namespace StdLib
{
    // make sure that RemoveListener is thread safe if you have listeners in different threads
    template <typename OwnerType, auto RemoveListener, typename IdType = ui32> class TListenerHandle
    {
        IdType _id{};
        std::weak_ptr<OwnerType> _owner{};

    public:
        using ownerType = OwnerType;
        using idType = IdType;

        ~TListenerHandle()
        {
            const auto &strongOwner = _owner.lock();
            if (strongOwner != nullptr)
            {
                RemoveListener(strongOwner.get(), this);
            }
        }

        TListenerHandle() = default;

        TListenerHandle(const std::weak_ptr<OwnerType> &owner, IdType id) : _owner(owner), _id(id)
        {}

        TListenerHandle(std::weak_ptr<OwnerType> &&owner, IdType id) : _owner(move(owner)), _id(id)
        {}

        TListenerHandle(TListenerHandle &&source) : _owner(move(source._owner)), _id(source._id)
        {
            source._owner = nullptr;
        }

        TListenerHandle &operator = (TListenerHandle &&source) noexcept
        {
            _owner = move(source._owner);
            _id = source._id;
            return *this;
        }

        bool operator == (const TListenerHandle &other) const
        {
            return _id == other._id && Funcs::AreSharedPointersEqual(_owner, other._owner);
        }

        bool operator != (const TListenerHandle &other) const
        {
            return !this->operator == (other);
        }

		bool operator < (const TListenerHandle &other) const
		{
			if (Funcs::AreSharedPointersEqual(_owner, other._owner))
				return _id < other._id;
			return _owner < other._owner;
		}

        IdType Id() const
        {
            return _id;
        }

        const std::weak_ptr<OwnerType> &Owner() const
        {
            return _owner;
        }

		std::weak_ptr<OwnerType> &Owner()
        {
            return _owner;
        }
    };

    // there's also a possible implementation which would patch relevant addresses
    // no reason to add it now, but it might be useful in the future because it
    // removes necessity to have a special shared object
}