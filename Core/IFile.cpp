#include "_PreHeader.hpp"
#include "IFile.hpp"

using namespace StdLib;

std::optional<ui32> IFile::WriteFormatted(PRINTF_VERIFY_FRONT const char *format, ...)
{
	va_list args;
	va_start(args, format);

	std::array<char, 4096> stackBuffer;
	std::unique_ptr<char[]> tempBuffer;
	char *targetBuffer = stackBuffer.data();

	int printed = vsnprintf(stackBuffer.data(), stackBuffer.size(), format, args);
	if (printed <= 0)
	{
		SOFTBREAK;
		return {};
	}
	if (static_cast<uiw>(printed) >= stackBuffer.size()) // stack buffer wasn't large enough
	{
		tempBuffer = std::make_unique<char[]>(printed + 1);
		targetBuffer = tempBuffer.get();
		printed = vsnprintf(targetBuffer, static_cast<uiw>(printed) + 1, format, args);
	}

	ui32 written;
	bool writeResult = Write(targetBuffer, static_cast<ui32>(printed), &written);

	va_end(args);

	if (writeResult)
	{
		return written;
	}
	return {};
}
