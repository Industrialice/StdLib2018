#pragma once

// TODO: add SAL annotations

extern "C" {

#ifndef PLATFORM_WINDOWS
	inline errno_t memcpy_s(void *RSTR destination, size_t destinationCapacity, const void *RSTR source, size_t size)
	{
		if (!destination && destinationCapacity)
		{
			return -1;
		}
		if (!source && size)
		{
			return -1;
		}
		if (destinationCapacity < size)
		{
			return -1;
		}
		memcpy(destination, source, size);
		return 0;
	}

	inline errno_t memmove_s(void *RSTR destination, size_t destinationCapacity, const void *RSTR source, size_t size)
	{
		if (!destination && destinationCapacity)
		{
			return -1;
		}
		if (!source && size)
		{
			return -1;
		}
		if (destinationCapacity < size)
		{
			return -1;
		}
		memmove(destination, source, size);
		return 0;
	}
#endif

inline errno_t memset_s(void *target, size_t destinationCapacity, int value, size_t size)
{
	if (destinationCapacity < size || !target)
	{
		return memcpy_s(nullptr, 1, nullptr, 1); // emulate a call of an actual _s function with wrong parameters
	}
	memset(target, value, size);
	return 0;
}

#ifndef STDLIB_DISABLE_MEMOPS_DEPRECATION
	namespace StdLib::__OriginalMemoryFuncs
	{
		inline void *_MemCpyOriginal(void *RSTR target, const void *source, uiw size)
		{
			return memcpy(target, source, size);
		}

		inline errno_t _MemCpySOriginal(void *RSTR destination, uiw destinationCapacity, const void *RSTR source, uiw size)
		{
			return memcpy_s(destination, destinationCapacity, source, size);
		}

		inline void *_MemMoveOriginal(void *RSTR target, const void *source, uiw size)
		{
			return memmove(target, source, size);
		}

		inline errno_t _MemMoveSOriginal(void *RSTR destination, uiw destinationCapacity, const void *RSTR source, uiw size)
		{
			return memmove_s(destination, destinationCapacity, source, size);
		}

		inline i32 _MemCmpOriginal(const void *target, const void *source, uiw size)
		{
			return memcmp(target, source, size);
		}

		inline void *_MemSetOriginal(void *target, i32 value, uiw size)
		{
			return memset(target, value, size);
		}

		inline errno_t _MemSetSOriginal(void *target, uiw destinationCapacity, i32 value, uiw size)
		{
			return memset_s(target, destinationCapacity, value, size);
		}

		inline const void *_MemChrOriginal(const void *target, i32 value, uiw size)
		{
			return memchr(target, value, size);
		}
	}

	#define _CallOriginalMemCpy StdLib::__OriginalMemoryFuncs::_MemCpyOriginal
	#define _CallOriginalMemCpyS StdLib::__OriginalMemoryFuncs::_MemCpySOriginal
	#define _CallOriginalMemMove StdLib::__OriginalMemoryFuncs::_MemMoveOriginal
	#define _CallOriginalMemMoveS StdLib::__OriginalMemoryFuncs::_MemMoveSOriginal
	#define _CallOriginalMemCmp StdLib::__OriginalMemoryFuncs::_MemCmpOriginal
	#define _CallOriginalMemSet StdLib::__OriginalMemoryFuncs::_MemSetOriginal
	#define _CallOriginalMemSetS StdLib::__OriginalMemoryFuncs::_MemSetSOriginal
	#define _CallOriginalMemChr StdLib::__OriginalMemoryFuncs::_MemChrOriginal

	namespace StdLib::__ReplacedMemoryFuncs
	{
		[[deprecated("memcpy is unsafe, use StdLib::MemOps::Copy instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline void *_MemCpy(void *RSTR target, const void *source, uiw size)
		{
			return memcpy(target, source, size);
		}

		[[deprecated("memcpy_s is unsafe, use StdLib::MemOps::CopyChecked instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline errno_t _MemCpyS(void *RSTR destination, uiw destinationCapacity, const void *RSTR source, uiw size)
		{
			return memcpy_s(destination, destinationCapacity, source, size);
		}

		[[deprecated("memmove is unsafe, use StdLib::MemOps::Move instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline void *_MemMove(void *RSTR target, const void *source, uiw size)
		{
			return memmove(target, source, size);
		}

		[[deprecated("memmove_s is unsafe, use StdLib::MemOps::MoveChecked instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline errno_t _MemMoveS(void *RSTR destination, uiw destinationCapacity, const void *RSTR source, uiw size)
		{
			return memmove_s(destination, destinationCapacity, source, size);
		}

		[[deprecated("memcmp is unsafe, use StdLib::MemOps::Compare instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline i32 _MemCmp(const void *target, const void *source, uiw size)
		{
			return memcmp(target, source, size);
		}

		[[deprecated("memset is unsafe, use StdLib::MemOps::Set instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline void *_MemSet(void *target, i32 value, uiw size)
		{
			return memset(target, value, size);
		}

		[[deprecated("memset_s is unsafe, use StdLib::MemOps::SetChecked instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline errno_t _MemSetS(void *target, uiw destinationCapacity, i32 value, uiw size)
		{
			return memset_s(target, destinationCapacity, value, size);
		}

		[[deprecated("memchr is unsafe, use StdLib::MemOps::Chr instead, or use STDLIB_DISABLE_MEMOPS_DEPRECATION to suppress this warning")]] inline const void *_MemChr(const void *target, i32 value, uiw size)
		{
			return memchr(target, value, size);
		}
	}

	#define memcpy StdLib::__ReplacedMemoryFuncs::_MemCpy
	#define memcpy_s StdLib::__ReplacedMemoryFuncs::_MemCpyS
	#define memmove StdLib::__ReplacedMemoryFuncs::_MemMove
	#define memmove_s StdLib::__ReplacedMemoryFuncs::_MemMoveS
	#define memcmp StdLib::__ReplacedMemoryFuncs::_MemCmp
	#define memset StdLib::__ReplacedMemoryFuncs::_MemSet
	#define memset_s StdLib::__ReplacedMemoryFuncs::_MemSetS
	#define memchr StdLib::__ReplacedMemoryFuncs::_MemChr
#else
	#define _CallOriginalMemCpy memcpy
	#define _CallOriginalMemCpyS memcpy_s
	#define _CallOriginalMemMove memmove
	#define _CallOriginalMemMoveS memmove_s
	#define _CallOriginalMemCmp memcmp
	#define _CallOriginalMemSet memset
	#define _CallOriginalMemSetS memset_s
	#define _CallOriginalMemChr memchr
#endif

} // finish extern "C"

namespace StdLib
{
	template <typename T, typename E> [[nodiscard]] constexpr bool _AreTypesCompatible()
	{
		using PT = std::remove_cvref_t<T>;
		using PE = std::remove_cvref_t<E>;
		if constexpr (std::is_same_v<PT, PE>)
		{
			return true;
		}
		else
		{
			constexpr bool isTBin = std::is_same_v<PT, std::byte> || std::is_same_v<PT, ui8> || std::is_same_v<PT, char> || std::is_same_v<PT, unsigned char>;
			constexpr bool isEBin = std::is_same_v<PE, std::byte> || std::is_same_v<PE, ui8> || std::is_same_v<PE, char> || std::is_same_v<PT, unsigned char>;
			return isTBin && isEBin;
		}
	}
}

// void * types are not allowed
// According to the docs target must always be a non-null pointer even when the size is 0, but
// ignore that rule as being detached from the real world
// TODO: check overlapping
namespace StdLib::MemOps
{
	template <typename T, typename E = T> T *Copy(T *RSTR destination, const E *RSTR source, uiw count)
	{
		static_assert(_AreTypesCompatible<T, E>(), "passed types are not compatible, consider using casts");
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME((destination && source) || count == 0);
		return static_cast<T *>(_CallOriginalMemCpy(destination, source, count * sizeof(T)));
	}

	template <typename T, typename E = T> bool CopyChecked(T *RSTR destination, uiw destinationCapacity, const E *RSTR source, uiw count)
	{
		static_assert(_AreTypesCompatible<T, E>(), "passed types are not compatible, consider using casts");
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		return 0 == _CallOriginalMemCpyS(destination, destinationCapacity * sizeof(T), source, count * sizeof(T));
	}

	template <typename T, typename E> T *Move(T *destination, const E *source, uiw count)
	{
		static_assert(_AreTypesCompatible<T, E>(), "passed types are not compatible, consider using casts");
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME((destination && source) || count == 0);
		return static_cast<T *>(_CallOriginalMemMove(destination, source, count * sizeof(T)));
	}

	template <typename T, typename E> bool MoveChecked(T *destination, uiw destinationCapacity, const E *source, uiw count)
	{
		static_assert(_AreTypesCompatible<T, E>(), "passed types are not compatible, consider using casts");
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		return 0 == _CallOriginalMemMoveS(destination, destinationCapacity * sizeof(T), source, count * sizeof(T));
	}

	template <typename T, typename E = T> [[nodiscard]] i32 Compare(const T *target, const E *source, uiw count)
	{
		static_assert(_AreTypesCompatible<T, E>(), "passed types are not compatible, consider using casts");
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME(target && source);
		return _CallOriginalMemCmp(target, source, count * sizeof(T));
	}

	template <typename T> T *Set(T *destination, ui8 value, uiw count)
	{
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME(destination || count == 0);
		return static_cast<T *>(_CallOriginalMemSet(destination, value, count * sizeof(T)));
	}

	template <typename T> bool SetChecked(T *destination, uiw destinationCapacity, ui8 value, uiw count)
	{
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		return 0 == _CallOriginalMemSetS(destination, destinationCapacity * sizeof(T), value, count * sizeof(T));
	}

	template <typename T> [[nodiscard]] const T *Chr(const T *source, ui8 value, uiw count)
	{
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME(source || count == 0);
		return static_cast<const T *>(_CallOriginalMemChr(source, value, count * sizeof(T)));
	}

	template <typename T> [[nodiscard]] T *Chr(T *source, ui8 value, uiw count)
	{
		static_assert(!std::is_same_v<T, void>, "void types are not allowed");
		ASSUME(source || count == 0);
		return const_cast<T *>(static_cast<const T *>(_CallOriginalMemChr(source, value, count * sizeof(T))));
	}
}

#undef _CallOriginalMemCpy
#undef _CallOriginalMemCpyS
#undef _CallOriginalMemMove
#undef _CallOriginalMemMoveS
#undef _CallOriginalMemCmp
#undef _CallOriginalMemSet
#undef _CallOriginalMemSetS
#undef _CallOriginalMemChr