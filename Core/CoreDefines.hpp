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
    #define ASSUME(condition) _ASSUME(condition)
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