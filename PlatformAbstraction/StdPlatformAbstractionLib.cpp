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

namespace StdLib::SystemInfo
{
	void Initialize();
}

namespace StdLib::Initialization
{
	void Initialize(const CoreParameters &parameters);
}

void StdLib::Initialization::Initialize(const PlatformAbstractionParameters &parameters)
{
    Initialize((const CoreParameters &)parameters);

    VirtualMemory::Initialize();
    FileInitialization::Initialize();
    TimeMomentInitialization::Initialize();
	SystemInfo::Initialize();
}