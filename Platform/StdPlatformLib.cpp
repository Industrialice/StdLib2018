#include "_PreHeader.hpp"
#include "_PlatformInitialization.hpp"

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
    Initialize(static_cast<const CoreParameters &>(parameters));

    VirtualMemory::Initialize();
    FileInitialization::Initialize();
    TimeMomentInitialization::Initialize();
	SystemInfo::Initialize();
}