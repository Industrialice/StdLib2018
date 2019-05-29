#pragma once

namespace StdLib::Initialization
{
	struct CoreParameters
	{
		std::function<void(const char *file, i32 line, i32 counter)> softBreakCallback{};
		f32 defaultF32Epsilon = 0.00001f;
	};

#ifndef _SUPPRESS_CORE_INITIALIZE
	void Initialize(const CoreParameters &parameters);
#endif
}