#include "_PreHeader.hpp"
#include "_MiscInitialization.hpp"

using namespace StdLib;

namespace StdLib::Initialization
{
	void Initialize(const PlatformParameters &parameters);
}

void Initialization::Initialize(const MiscParameters &parameters)
{
    Initialize((const PlatformParameters &)parameters);
}
