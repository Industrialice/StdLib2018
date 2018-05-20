#pragma once

#include "GenericFuncs.hpp"
#include "Result.hpp"
#include "Allocators.hpp"
#include "IMemoryStream.hpp"
#include "IFile.hpp"

namespace StdLib::Initialization
{
    struct CoreParameters
    {
        std::function<void(const char *file, i32 line, i32 counter)> softBreakCallback{};
        f32 defaultF32Epsilon = 0.000001f;
    };

    void CoreInitialize(const CoreParameters &parameters);
}