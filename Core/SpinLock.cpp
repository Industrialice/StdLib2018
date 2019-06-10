#include "_PreHeader.hpp"
#include "SpinLock.hpp"

using namespace StdLib;

SpinLock::SpinLock(SpinLock &&source) noexcept
{
	ASSUME(source._users.load() == 0 && _users.load() == 0); // can't move while locked
}

SpinLock &SpinLock::operator = (SpinLock &&source) noexcept
{
	ASSUME(source._users.load() == 0); // can't move while locked
	_users.store(0);
	return *this;
}

auto SpinLock::Lock() const -> Unlocker
{
	lock();
	return {*this};
}

auto StdLib::SpinLock::TryLock() const -> std::optional<Unlocker>
{
	if (try_lock())
	{
		return Unlocker(*this);
	}
	return std::nullopt;
}

void SpinLock::Unlock() const
{
	unlock();
}

void SpinLock::lock() const
{
	for (;;)
	{
		atomicType oldLock = 0;
		if (_users.compare_exchange_weak(oldLock, 1, std::memory_order_acquire))
		{
			break;
		}
	}
}

bool SpinLock::try_lock() const
{
	atomicType oldLock = 0;
	return _users.compare_exchange_strong(oldLock, 1, std::memory_order_acquire);
}

void SpinLock::unlock() const
{
	ASSUME(_users.load() == 1);
	_users.store(0, std::memory_order_release);
}

SpinLock::Unlocker::Unlocker(const SpinLock &lock) noexcept : _lock(&lock)
{
}

SpinLock::Unlocker::~Unlocker() noexcept
{
	ASSUME_DEBUG_ONLY(_lock == nullptr);
}

SpinLock::Unlocker::Unlocker(Unlocker &&source) noexcept : _lock(source._lock)
{
	ASSUME(this != &source);
#ifdef DEBUG
	source._lock = nullptr;
#endif
}

auto SpinLock::Unlocker::operator = (Unlocker &&source) noexcept -> Unlocker &
{
	ASSUME(this != &source);
	_lock = source._lock;
#ifdef DEBUG
	source._lock = nullptr;
#endif
	return *this;
}

void SpinLock::Unlocker::Unlock()
{
	ASSUME(_lock != nullptr);
	_lock->Unlock();
#ifdef DEBUG
	_lock = nullptr;
#endif
}