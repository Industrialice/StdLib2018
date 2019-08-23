#pragma once

namespace StdLib
{
	template <typename T> struct MovableAtomic : public std::atomic<T>
	{
		using std::atomic<T>::atomic;

		static_assert(std::atomic<T>::is_always_lock_free);

		MovableAtomic(MovableAtomic &&source) noexcept
		{
			this->store(source.load());
		}

		MovableAtomic &operator = (MovableAtomic &&source) noexcept
		{
			this->store(source.load());
			return *this;
		}
	};

	template <typename T> struct _BaseStringViewNullTerminated : public std::basic_string_view<T>
	{
		static constexpr T empty[] = {T('\0')};

		_BaseStringViewNullTerminated() : std::basic_string_view<T>(empty, 0)
		{}

		_BaseStringViewNullTerminated(const std::basic_string<T> &source) : std::basic_string_view<T>(source)
		{
		}

		_BaseStringViewNullTerminated(std::basic_string<T> &&) noexcept = delete;

		_BaseStringViewNullTerminated(std::basic_string_view<T> source) : std::basic_string_view<T>(source)
		{
			ASSUME(source.data());
			ASSUME(source.data()[source.size()] == T('\0'));
		}

		_BaseStringViewNullTerminated(const T *source, uiw count) : std::basic_string_view<T>(source, count)
		{
			ASSUME(source);
			ASSUME(source[count] == T('\0'));
		}

		_BaseStringViewNullTerminated(const _BaseStringViewNullTerminated &) = default;

		_BaseStringViewNullTerminated(_BaseStringViewNullTerminated &&) = default;

		_BaseStringViewNullTerminated &operator = (const _BaseStringViewNullTerminated &) = default;

		_BaseStringViewNullTerminated &operator = (_BaseStringViewNullTerminated &&) noexcept = default;

		_BaseStringViewNullTerminated &operator = (const std::basic_string<T> &source)
		{
			std::basic_string_view<T>::operator=(source);
			return *this;
		}

		_BaseStringViewNullTerminated &operator = (std::basic_string<T> &&source) noexcept = delete;

		_BaseStringViewNullTerminated &operator = (std::basic_string_view<T> source)
		{
			ASSUME(source.data());
			ASSUME(source.data()[source.size()] == T('\0'));
			std::basic_string_view<T>::operator=(source);
			return *this;
		}
	};

	struct StringViewNullTerminated : _BaseStringViewNullTerminated<char>
	{
		using _BaseStringViewNullTerminated::_BaseStringViewNullTerminated;
	};
}