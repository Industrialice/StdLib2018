#pragma once

namespace StdLib
{
	[[nodiscard]] NOINLINE Error<> PlatformErrorResolve(const char *message = nullptr);
}