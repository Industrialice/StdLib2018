#include "_PreHeader.hpp"
#include "StdPlatformLib.hpp"

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

void StdLib::Initialization::Initialize(const PlatformParameters &parameters)
{
    Initialize((const CoreParameters &)parameters);

    VirtualMemory::Initialize();
    FileInitialization::Initialize();
    TimeMomentInitialization::Initialize();
	SystemInfo::Initialize();
}