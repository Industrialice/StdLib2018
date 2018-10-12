#pragma once

#include <StdPlatformAbstractionLib.hpp>
#include "UniqueIdManager.hpp"

namespace StdLib::Initialization
{
    struct MiscellaneousParameters : PlatformAbstractionParameters
    {
    };

    void MiscellaneousInitialize(const MiscellaneousParameters &parameters);
}