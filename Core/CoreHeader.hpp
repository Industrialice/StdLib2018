#pragma once

#define _FILE_OFFSET_BITS 64

#include <memory>
#include <type_traits>
#include <functional>
#include <string>
#include <string_view>
#include <array>
#include <iterator>
#include <vector>
#include <variant>
#include <malloc.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std::literals;

#if __has_include(<optional>)
    #include <optional>
#else
    #warning This system doesn't provide unexperimental optional
    #include <experimental/optional>
    namespace std
    {
        using std::experimental::optional;
        using std::experimental::nullopt;
    }
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#include "CompilerSpecific.hpp"
#include "CoreTypes.hpp"
#include "PlatformDefines.hpp"
#include "CoreDefines.hpp"
#include "MemOps.hpp"

#ifdef SPACESHIP_SUPPORTED
	#include <compare>
#endif

namespace StdLib
{
    extern f32 DefaultF32Epsilon;
}

template <typename...> inline constexpr bool false_v = false;