#pragma once

namespace StdLib
{
	class SpinLock
	{
		friend class Unlocker;

		using atomicType = ui32;

		alignas(64) mutable std::atomic<atomicType> _users{0};

	public:
		class Unlocker
		{
			friend SpinLock;

			const SpinLock *_lock;

			Unlocker(const SpinLock &lock) noexcept;

		public:
			~Unlocker() noexcept;
			Unlocker(Unlocker &&source) noexcept;
			Unlocker &operator = (Unlocker &&source) noexcept;
			void Unlock();
		};

		SpinLock() noexcept = default;
		SpinLock(SpinLock &&source) noexcept;
		SpinLock &operator = (SpinLock &&source) noexcept;

		[[nodiscard]] Unlocker Lock() const;
		[[nodiscard]] std::optional<Unlocker> TryLock() const;

		void lock() const;
		[[nodiscard]] bool try_lock() const;
		void unlock() const;

	private:
		void Unlock() const;
	};
}