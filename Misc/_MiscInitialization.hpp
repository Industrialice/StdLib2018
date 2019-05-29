#pragma once

namespace StdLib::Initialization
{
	struct MiscParameters : PlatformParameters
	{
	};

#ifndef _SUPPRESS_MISC_INITIALIZE
	void Initialize(const MiscParameters &parameters);
#endif
}