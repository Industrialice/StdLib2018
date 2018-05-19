#include "StdPlatformAbstractionLib.hpp"

namespace StdLib::VirtualMemory
{
    void Initialize();
}

namespace StdLib::FileInitialization
{
    void Initialize();
}

void StdLib::Initialization::PlatformAbstractionInitialize(const PlatformAbstractionParameters &parameters)
{
    CoreInitialize(parameters.coreParameters);

    VirtualMemory::Initialize();
    FileInitialization::Initialize();
}