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

	struct StringViewNullTerminated : public std::basic_string_view<char>
	{
		StringViewNullTerminated() : std::basic_string_view<char>("", 0)
		{}

		StringViewNullTerminated(const std::string &source) : std::basic_string_view<char>(source)
		{
		}

		StringViewNullTerminated(std::string &&) noexcept = delete;

		StringViewNullTerminated(std::string_view source) : std::basic_string_view<char>(source)
		{
			ASSUME(source.data()[source.size()] == '\0');
		}

		StringViewNullTerminated(const char *source, uiw count) : std::basic_string_view<char>(source, count)
		{
			ASSUME(source[count] == '\0');
		}

		StringViewNullTerminated(const StringViewNullTerminated &) = default;

		StringViewNullTerminated(StringViewNullTerminated &&) = default;

		StringViewNullTerminated &operator = (const StringViewNullTerminated &) = default;

		StringViewNullTerminated &operator = (StringViewNullTerminated &&) noexcept = default;

		StringViewNullTerminated &operator = (const std::string &source)
		{
			std::basic_string_view<char>::operator=(source);
			return *this;
		}

		StringViewNullTerminated &operator = (std::string &&source) noexcept = delete;

		StringViewNullTerminated &operator = (std::string_view source)
		{
			ASSUME(source.data()[source.size()] == '\0');
			std::basic_string_view<char>::operator=(source);
			return *this;
		}
	};
}