#pragma once

#include "FilePath.hpp"
#include "VirtualMemory.hpp"

namespace StdLib::Initialization
{
    struct PlatformAbstractionParameters
    {
        CoreParameters coreParameters{};
    };

    void PlatformAbstractionInitialize(const PlatformAbstractionParameters &parameters);
}