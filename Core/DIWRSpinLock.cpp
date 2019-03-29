#include "_PreHeader.hpp"
#include "DIWRSpinLock.hpp"
#include "GenericFuncs.hpp"

using namespace StdLib;

// TODO: optimize memory_order

DIWRSpinLock::DIWRSpinLock(DIWRSpinLock &&source) noexcept
{
    ASSUME(source._users.load() == 0);
}

DIWRSpinLock &DIWRSpinLock::operator = (DIWRSpinLock &&source) noexcept
{
    ASSUME(this != &source);
    ASSUME(source._users.load() == 0);
    _users.store(0);
    return *this;
}

auto DIWRSpinLock::Lock(LockType type) const -> Unlocker
{
    switch (type)
    {
    case DIWRSpinLock::LockType::Read: // exclusive and pending exclusive must be 0, inclusive and read don't matter
        for (auto oldLock = _users.load();;)
        {
            oldLock &= ReadersMask | InclusiveMask;
            auto newLock = oldLock + 1;
            if (_users.compare_exchange_weak(oldLock, newLock))
            {
                break;
            }
        }
        break;
    case DIWRSpinLock::LockType::Exclusive: // inclusive, exclusive and read must be 0, pending exclusive doesn't matter
        _users.fetch_add(ReadersMask + 1);
        for (auto oldLock = _users.load();;)
        {
            oldLock &= PendingExclusiveMask;
            auto newLock = ExclusiveMask | oldLock;
            if (_users.compare_exchange_weak(oldLock, newLock))
            {
                break;
            }
        }
        break;
    case DIWRSpinLock::LockType::Inclusive: // inclusive, exclusive and pending exclusive must be 0, read doesn't matter
        for (auto oldLock = _users.load();;)
        {
            oldLock &= ReadersMask;
            auto newLock = InclusiveMask | oldLock;
            if (_users.compare_exchange_weak(oldLock, newLock))
            {
                break;
            }
        }
        break;
    }

    return Unlocker(*this, type);
}

auto DIWRSpinLock::TryLock(LockType type) const -> std::optional<Unlocker>
{
    atomicType oldLock, newLock;

    switch (type)
    {
    case DIWRSpinLock::LockType::Read: // exclusive and pending exclusive must be 0, inclusive and read don't matter
        oldLock = _users.load();
        oldLock &= ReadersMask | InclusiveMask;
        newLock = oldLock + 1;
        if (_users.compare_exchange_strong(oldLock, newLock))
        {
            return Unlocker(*this, type);
        }
        break;
    case DIWRSpinLock::LockType::Exclusive: // inclusive, exclusive, read and pending exclusive must be 0
        oldLock = 0;
        newLock = ExclusiveMask | (ReadersMask + 1);
        if (_users.compare_exchange_strong(oldLock, newLock))
        {
            return Unlocker(*this, type);
        }
        break;
    case DIWRSpinLock::LockType::Inclusive: // inclusive, exclusive and pending exclusive must be 0, read doesn't matter
        oldLock = _users.load();
        oldLock &= ReadersMask;
        newLock = InclusiveMask | oldLock;
        if (_users.compare_exchange_strong(oldLock, newLock))
        {
            return Unlocker(*this, type);
        }
        break;
    }

    return std::nullopt;
}

void DIWRSpinLock::Unlock(LockType type) const
{
    switch (type)
    {
    case DIWRSpinLock::LockType::Read:
        ASSUME((_users.load() & ReadersMask) > 0);
        ASSUME(!Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));
        _users.fetch_sub(1); // decrement readers counter
        break;
    case DIWRSpinLock::LockType::Exclusive:
        ASSUME(Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));
        ASSUME(!Funcs::IsBitSet(_users.load(), InclusiveBitIndex));
        ASSUME((_users.load() & ReadersMask) == 0);
        ASSUME((_users.load() & PendingExclusiveMask) > 0);
        _users.fetch_sub((ReadersMask + 1) | ExclusiveMask); // remove exclusive flag, decrement pending exclusive counter
        break;
    case DIWRSpinLock::LockType::Inclusive:
        ASSUME(!Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));
        ASSUME(Funcs::IsBitSet(_users.load(), InclusiveBitIndex));
        _users.fetch_sub(InclusiveMask); // remove inclusive flag
        break;
    }
}

void DIWRSpinLock::Transition(LockType current, LockType target) const
{
    switch (current)
    {
    case LockType::Exclusive:
        ASSUME(Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));
        ASSUME(!Funcs::IsBitSet(_users.load(), InclusiveBitIndex));
        ASSUME((_users.load() & ReadersMask) == 0);
        ASSUME((_users.load() & PendingExclusiveMask) > 0);

        switch (target)
        {
        case LockType::Exclusive:
            SOFTBREAK;
            break;
        case LockType::Inclusive:
            _users.fetch_add(InclusiveMask - ((ReadersMask + 1) | ExclusiveMask)); // remove exclusive flag, decrement pending exclusive counter, set inclusive flag
            break;
        case LockType::Read:
            _users.fetch_add(1 - ((ReadersMask + 1) | ExclusiveMask)); // remove exclusive flag, decrement pending exclusive counter, increment readers counter
            break;
        }
        break;
    case LockType::Inclusive:
        ASSUME(Funcs::IsBitSet(_users.load(), InclusiveBitIndex));
        ASSUME(!Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));

        switch (target)
        {
        case LockType::Exclusive:
            _users.fetch_add(ReadersMask + 1); // increment pending exclusive counter
            for (auto oldLock = _users.load();;)
            {
                oldLock &= PendingExclusiveMask;
                auto newLock = oldLock | ExclusiveMask;
                oldLock |= InclusiveMask;
                if (_users.compare_exchange_weak(oldLock, newLock))
                {
                    break;
                }
            }
            break;
        case LockType::Inclusive:
            SOFTBREAK;
            break;
        case LockType::Read:
            _users.fetch_add(1 - InclusiveMask); // remove inclusive flag, increment readers counter
            break;
        }
		break;
    case LockType::Read:
        ASSUME((_users.load() & ReadersMask) > 0);
        ASSUME(!Funcs::IsBitSet(_users.load(), ExclusiveBitIndex));

        switch (target)
        {
        case LockType::Exclusive:
            _users.fetch_add(ReadersMask + 1); // increment pending exclusive counter
            for (auto oldLock = _users.load();;)
            {
                oldLock &= PendingExclusiveMask;
                auto newLock = oldLock | ExclusiveMask;
                oldLock |= 1;
                if (_users.compare_exchange_weak(oldLock, newLock))
                {
                    break;
                }
            }
            break;
        case LockType::Inclusive:
            for (auto oldLock = _users.load();;)
            {
                oldLock &= ReadersMask;
                auto newLock = InclusiveMask | (oldLock - 1);
                if (_users.compare_exchange_weak(oldLock, newLock))
                {
                    break;
                }
            }
            break;
        case LockType::Read:
            SOFTBREAK;
            break;
        }
		break;
    }
}

DIWRSpinLock::Unlocker::Unlocker(const DIWRSpinLock &lock, DIWRSpinLock::LockType lockType) noexcept : _lock(&lock), _lockType(lockType)
{}

DIWRSpinLock::Unlocker::~Unlocker() noexcept
{
#ifdef DEBUG
    ASSUME(_lock == nullptr);
#endif
}

DIWRSpinLock::Unlocker::Unlocker(Unlocker &&source) noexcept : _lock(source._lock), _lockType(source._lockType)
{
#ifdef DEBUG
    source._lock = nullptr;
#endif
}

auto DIWRSpinLock::Unlocker::operator = (Unlocker &&source) noexcept -> Unlocker &
{
#ifdef DEBUG
    ASSUME(_lock == nullptr);
#endif

    ASSUME(this != &source);
    _lock = source._lock;
    _lockType = source._lockType;

#ifdef DEBUG
    source._lock = nullptr;
#endif

    return *this;
}

void DIWRSpinLock::Unlocker::Unlock()
{
    _lock->Unlock(_lockType);

#ifdef DEBUG
    ASSUME(_lock);
    _lock = nullptr;
#endif
}

void DIWRSpinLock::Unlocker::Transition(DIWRSpinLock::LockType target)
{
#ifdef DEBUG
    ASSUME(_lock);
#endif
    _lock->Transition(_lockType, target);
    _lockType = target;
}

auto DIWRSpinLock::Unlocker::LockType() const -> DIWRSpinLock::LockType
{
    return _lockType;
}

bool DIWRSpinLock::Unlocker::PointToSameLock(const Unlocker &other) const
{
#ifdef DEBUG
    ASSUME(_lock && other._lock);
#endif
    return _lock == other._lock;
}
