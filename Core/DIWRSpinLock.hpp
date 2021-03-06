#pragma once

namespace StdLib
{
	/* This lock supports 3 types of locking:
	   Read: will wait if it's Exclusively locked, otherwise works with
	     other types of locks.
	   Exclusive: doesn't allow any other locks to be held simultaneously
	   Inclusive: only one Inclusive lock can be held at a time, but it doesn't
	     preclude Read locks. Can be promoted to Exclusive.
	*/
    class DIWRSpinLock
    {
        friend class Unlocker;

        using atomicType = ui32;

        static constexpr atomicType InclusiveBitIndex = 31;
        static constexpr atomicType InclusiveMask = 1 << 31;
        static constexpr atomicType ExclusiveBitIndex = 30;
        static constexpr atomicType ExclusiveMask = 1 << 30;
        static constexpr atomicType PendingExclusiveMask = 0x3FFF'0000;
        static constexpr atomicType ReadersMask = 0xFFFF;

		alignas(64) mutable std::atomic<atomicType> _users{0};

    public:
        enum class LockType { Read, Exclusive, Inclusive };

        class Unlocker
        {
            friend DIWRSpinLock;

            LockType _lockType;
            const DIWRSpinLock *_lock;

            Unlocker(const DIWRSpinLock &lock, LockType lockType) noexcept;

        public:
            ~Unlocker() noexcept;
            Unlocker(Unlocker &&source) noexcept;
            Unlocker &operator = (Unlocker &&source) noexcept;
            void Unlock();
            void Transition(LockType target);
            [[nodiscard]] LockType LockType() const;
            [[nodiscard]] bool PointToSameLock(const Unlocker &other) const;
        };

        DIWRSpinLock() noexcept = default;
        DIWRSpinLock(DIWRSpinLock &&source) noexcept;
        DIWRSpinLock &operator = (DIWRSpinLock &&source) noexcept;

        [[nodiscard]] Unlocker Lock(LockType type) const;
        [[nodiscard]] std::optional<Unlocker> TryLock(LockType type) const; // avoid using it with LockType::Exclusive, there may always be some read/inclusive locks that will prevent it from ever succeeding

    private:
        void Unlock(LockType type) const;
        void Transition(LockType current, LockType target) const;
    };
}