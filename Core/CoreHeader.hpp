#pragma once

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

using namespace std::literals::string_literals;

#if __has_include(<optional>)
    #include <optional>
    using std::optional;
    using std::nullopt;
#else
    #warning The system doesn't provide unexperimental optional
    #include <experimental/optional>
    using std::experimental::optional;
    using std::experimental::nullopt;
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#include "CompilerSpecific.hpp"
#include "CoreTypes.hpp"
#include "PlatformDefines.hpp"
#include "CoreDefines.hpp"

namespace StdLib
{
    extern f32 DefaultF32Epsilon;
}