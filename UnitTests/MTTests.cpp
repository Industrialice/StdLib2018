#include "_PreHeader.hpp"
#include <DIWRSpinLock.hpp>

using namespace StdLib;
using std::nullopt;
using Funcs::Reinitialize;

static void SpinLockTests()
{
	DIWRSpinLock spin;
	auto attemptReadLocks = [&spin](uiw count, bool expectToSucceed)
	{
		auto locks = ALLOCA_TYPED(count, std::optional<DIWRSpinLock::Unlocker>);
		for (uiw index = 0; index < count; ++index)
		{
			new (&locks[index]) std::optional<DIWRSpinLock::Unlocker>(spin.TryLock(DIWRSpinLock::LockType::Read));
			if (expectToSucceed)
			{
				UTest(NotEqual, locks[index], nullopt);
			}
			else
			{
				UTest(Equal, locks[index], nullopt);
			}
		}
		for (uiw index = 0; index < count; ++index)
		{
			if (expectToSucceed)
			{
				locks[index]->Unlock();
			}
			locks[index]->~Unlocker();
		}
	};

	auto lock = spin.Lock(DIWRSpinLock::LockType::Read);
	std::optional<DIWRSpinLock::Unlocker> lock2, lock3;

	auto testForRead = [&spin, &lock, &lock2, &lock3, attemptReadLocks]
	{
		attemptReadLocks(10, true);
		Reinitialize(lock2, spin.TryLock(DIWRSpinLock::LockType::Inclusive));
		UTest(NotEqual, lock2, nullopt);
		Reinitialize(lock3, spin.TryLock(DIWRSpinLock::LockType::Inclusive));
		UTest(Equal, lock3, nullopt);
		Reinitialize(lock3, spin.TryLock(DIWRSpinLock::LockType::Exclusive));
		UTest(Equal, lock3, nullopt);
		lock2->Unlock();
		Reinitialize(lock3, spin.TryLock(DIWRSpinLock::LockType::Exclusive));
		UTest(Equal, lock3, nullopt);
	};
	auto testForInclusive = [&spin, &lock, &lock2, attemptReadLocks]
	{
		attemptReadLocks(10, true);
		Reinitialize(lock2, spin.TryLock(DIWRSpinLock::LockType::Inclusive));
		UTest(Equal, lock2, nullopt);
		Reinitialize(lock2, spin.TryLock(DIWRSpinLock::LockType::Exclusive));
		UTest(Equal, lock2, nullopt);
	};
	auto testForExclusive = [&spin, &lock, &lock2, attemptReadLocks]
	{
		attemptReadLocks(10, false);
		Reinitialize(lock2, spin.TryLock(DIWRSpinLock::LockType::Inclusive));
		UTest(Equal, lock2, nullopt);
		Reinitialize(lock2, spin.TryLock(DIWRSpinLock::LockType::Exclusive));
		UTest(Equal, lock2, nullopt);
	};

	testForRead();
	lock.Unlock();
	Reinitialize(lock3, spin.TryLock(DIWRSpinLock::LockType::Exclusive));
	UTest(NotEqual, lock3, nullopt);
	lock3->Unlock();

	Reinitialize(lock, spin.Lock(DIWRSpinLock::LockType::Inclusive));
	testForInclusive();
	lock.Unlock();

	Reinitialize(lock, spin.Lock(DIWRSpinLock::LockType::Exclusive));
	testForExclusive();

	lock.Transition(DIWRSpinLock::LockType::Inclusive); // Exclusive->Inclusive
	testForInclusive();
	lock.Transition(DIWRSpinLock::LockType::Exclusive); // Inclusive->Exclusive
	testForExclusive();
	lock.Transition(DIWRSpinLock::LockType::Read); // Exclusive->Read
	testForRead();
	lock.Transition(DIWRSpinLock::LockType::Inclusive); // Read->Inclusive
	testForInclusive();
	lock.Transition(DIWRSpinLock::LockType::Read); // Inclusive->Read
	testForRead();
	lock.Transition(DIWRSpinLock::LockType::Exclusive); // Read->Exclusive
	testForExclusive();

	lock.Unlock();
}

void MTTests()
{
	SpinLockTests();

    Logger::Message("finished multithreaded tests\n");
}