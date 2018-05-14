#include "StdPlatformAbstractionLib.hpp"

namespace StdLib::VirtualMemory
{
    void Initialize();
}

void StdLib::Initialization::PlatformAbstractionInitialize(const PlatformAbstractionParameters &parameters)
{
    CoreInitialize(parameters.coreParameters);

    VirtualMemory::Initialize();
}
