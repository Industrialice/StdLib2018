#pragma once

#include "EnumCombinable.hpp"
#include "CompileTimeStrings.hpp"
#include "GenericFuncs.hpp"
#include "HashFuncs.hpp"
#include "TypeIdentifiable.hpp"
#include "Result.hpp"
#include "Allocators.hpp"
#include "IMemoryStream.hpp"
#include "IFile.hpp"
#include "DataHolder.hpp"

namespace StdLib::Initialization
{
    struct CoreParameters
    {
        std::function<void(const char *file, i32 line, i32 counter)> softBreakCallback{};
        f32 defaultF32Epsilon = 0.00001f;
    };

#ifndef _SUPPRESS_CORE_INITIALIZE
    void Initialize(const CoreParameters &parameters);
#endif
}

#define _STD_CORE_LIB_INCLUDED