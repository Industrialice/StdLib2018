#pragma once

#include "FilePath.hpp"
#include "VirtualMemory.hpp"
#include "TimeMoment.hpp"
#include "File.hpp"
#include "FileSystem.hpp"
#include "MemoryMappedFile.hpp"

namespace StdLib::Initialization
{
    struct PlatformAbstractionParameters : CoreParameters
    {
    };

    void PlatformAbstractionInitialize(const PlatformAbstractionParameters &parameters);
}