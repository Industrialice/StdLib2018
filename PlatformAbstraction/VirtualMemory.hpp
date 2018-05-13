#pragma once

#include <StdCoreLib.hpp>

namespace StdLib::VirtualMemory
{
    enum class PageMode : ui32
    {
        Write = Funcs::BitPos(0),
        Read = Funcs::BitPos(1),
        Execute = Funcs::BitPos(2)
    };
}

namespace StdLib
{
    ENUM_COMBINABLE(VirtualMemory::PageMode, ui32)
}