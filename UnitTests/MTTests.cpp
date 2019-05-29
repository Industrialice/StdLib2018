#include "_PreHeader.hpp"

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
            std::exchange(locks[index], {});
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

struct MessageQueueTestStruct
{
	void Call(ui32 &calledTimes)
	{
		++calledTimes;
	}
};

static void MessageQueueTestFunc(ui32 &calledTimes)
{
	++calledTimes;
}

static void MessageQueueTests()
{
	MTMessageQueue messageQueue;

	ui32 calledTimes = 0;

	auto toCall = [](ui32 &calledTimes)
	{
		++calledTimes;
	};

	auto toCallConstRef = [](const ui32 &calledTimes)
	{};

	auto toCallPtr = [](ui32 *calledTimes)
	{
		++*calledTimes;
	};

	auto toCallConstPtr = [](const ui32 *calledTimes)
	{};

	auto funcWithUniqueArgument = [](std::unique_ptr<int> arg)
	{
		UTest(Equal, *arg, 15);
	};

	auto funcWithArrayRef = [](int(&arr)[10])
	{
		for (uiw index = 1; index < 10; ++index)
		{
			UTest(Equal, arr[index - 1], index);
		}
	};

	auto funcWithArray = [](int *arr)
	{
		for (uiw index = 1; index < 10; ++index)
		{
			UTest(Equal, arr[index - 1], index);
		}
	};

	int arr[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	MessageQueueTestStruct object;

	auto sharedObject = std::make_shared<MessageQueueTestStruct>();
	auto uniqueObject = std::make_unique<MessageQueueTestStruct>();

	auto uniqueArgument = std::make_unique<int>(15);

	messageQueue.Add(toCall, std::ref(calledTimes));
	messageQueue.Add(toCallConstRef, std::cref(calledTimes));
	messageQueue.Add(toCallConstRef, std::ref(calledTimes));
	messageQueue.Add(toCallConstRef, calledTimes);
	messageQueue.Add(toCall, calledTimes); // don't pass as a reference, should create a copy
	messageQueue.Add(toCallPtr, &calledTimes);
	messageQueue.Add(toCallConstPtr, &calledTimes);
	messageQueue.Add(funcWithUniqueArgument, move(uniqueArgument));
	messageQueue.Add(funcWithArray, arr);
	messageQueue.Add(funcWithArrayRef, std::ref(arr));
	messageQueue.Add<MessageQueueTestFunc>(std::ref(calledTimes));
	messageQueue.Add<&MessageQueueTestStruct::Call>(object, std::ref(calledTimes));
	messageQueue.Add<&MessageQueueTestStruct::Call>(sharedObject, std::ref(calledTimes));
	messageQueue.Add<&MessageQueueTestStruct::Call>(uniqueObject, std::ref(calledTimes));
	messageQueue.Add<&MessageQueueTestStruct::Call>(move(uniqueObject), std::ref(calledTimes));
	
	while (messageQueue.ExecNoWait());

	UTest(Equal, calledTimes, 7);
}

void MTTests()
{
	SpinLockTests();
	MessageQueueTests();

    UnitTestsLogger::Message("finished multithreaded tests\n");
}