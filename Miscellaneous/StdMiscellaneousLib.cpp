#include "_PreHeader.hpp"
#include "StdMiscellaneousLib.hpp"

using namespace StdLib;

namespace StdLib::Initialization
{
	void Initialize(const PlatformAbstractionParameters &parameters);
}

void Initialization::Initialize(const MiscellaneousParameters &parameters)
{
    Initialize((const PlatformAbstractionParameters &)parameters);
}
