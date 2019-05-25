#pragma once

namespace StdLib::_Private
{
    void SoftBreak(const char *file, i32 line, i32 counter);
}

#ifdef DEBUG
    #define HARDBREAK _RAISE_EXCEPTION
    #define SOFTBREAK _Private::SoftBreak(__FILE__, __LINE__, __COUNTER__)
    #define UNREACHABLE _RAISE_EXCEPTION
    #ifdef PLATFORM_EMSCRIPTEN
        #define ASSUME(condition) do { if (!(condition)) { printf("!!!ASSUMPTION %s FAILED!!!\n", TOSTR(condition)); HARDBREAK; } } while(0)
    #else
        #define ASSUME(condition) do { if (!(condition)) HARDBREAK; } while(0)
    #endif
#else
    #define HARDBREAK _UNREACHABLE
    #define SOFTBREAK
    #define UNREACHABLE _UNREACHABLE
	#ifdef STDLIB_ENABLE_ASSUME_RELEASE_CHECKS
		#define ASSUME(condition) SOFTBREAK
	#else
		#define ASSUME(condition) _ASSUME(condition)
	#endif
#endif

#define NOIMPL HARDBREAK

// CONCAT_EXPANDED first expands the arguments, for example possible output of CONCAT_EXPANDED(__FILE__, __LINE__) is Test.cpp107
// CONCAT(__FILE__, __LINE__) will output __FILE____LINE__
#define CONCAT_EXPANDED(first, second) CONCAT(first, second)
#define CONCAT(first, second) first##second
#define TOSTR(code) #code
#define ALLOCA_TYPED(count, type) (std::remove_const_t<type> *)ALLOCA(count, sizeof(type))

#define static_warning(condition, message) \
	struct CONCAT_EXPANDED(_Warning, __LINE__) \
	{ \
		constexpr DEPRECATE(void _(std::false_type), message) {}; \
		constexpr void _(std::true_type) {}; \
		constexpr CONCAT_EXPANDED(_Warning, __LINE__)() {_(std::conditional_t<condition, std::true_type, std::false_type>());} \
	}

namespace StdLib
{
	template <typename Tin> [[nodiscard]] constexpr uiw _CountOf()
	{
		using T = typename std::remove_reference_t<Tin>;
		static_assert(std::is_array_v<T>, "CountOf() requires an array argument");
		static_assert(std::extent_v<T> > 0, "zero- or unknown-size array");
		return std::extent_v<T>;
	}
}

#define CountOf(a) StdLib::_CountOf<decltype(a)>()

#ifndef HAS_MAKE_ARRAY
	// temporary emulation of std::make_array, taken from https://en.cppreference.com/w/cpp/experimental/make_array
	namespace _details
	{
		template<class> struct is_ref_wrapper : std::false_type {};
		template<class T> struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

		template<class T>
		using not_ref_wrapper = std::negation<is_ref_wrapper<std::decay_t<T>>>;

		template <class D, class...> struct return_type_helper { using type = D; };
		template <class... Types>
		struct return_type_helper<void, Types...> : std::common_type<Types...>
		{
			static_assert(std::conjunction_v<not_ref_wrapper<Types>...>,
				"Types cannot contain reference_wrappers when D is void");
		};

		template <class D, class... Types>
		using return_type = std::array<typename return_type_helper<D, Types...>::type, sizeof...(Types)>;
	}

	namespace std
	{
		template <class D = void, class... Types> [[nodiscard]] constexpr _details::return_type<D, Types...> make_array(Types &&... t)
		{
			return { std::forward<Types>(t)... };
		}
	}
#endif