#pragma once

namespace StdLib::Initialization
{
	struct PlatformParameters : CoreParameters
	{
	};

#ifndef _SUPPRESS_PLATFORM_INITIALIZE
	void Initialize(const PlatformParameters &parameters);
#endif
}