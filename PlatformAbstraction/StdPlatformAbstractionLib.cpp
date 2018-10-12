#include "_PreHeader.hpp"
#include "StdPlatformAbstractionLib.hpp"

namespace StdLib::VirtualMemory
{
    void Initialize();
}

namespace StdLib::FileInitialization
{
    void Initialize();
}

namespace StdLib::TimeMomentInitialization
{
    void Initialize();
}

void StdLib::Initialization::PlatformAbstractionInitialize(const PlatformAbstractionParameters &parameters)
{
    CoreInitialize(parameters.coreParameters);

    VirtualMemory::Initialize();
    FileInitialization::Initialize();
    TimeMomentInitialization::Initialize();
}