#include "_PreHeader.hpp"
#include "NativeConsole.hpp"

using namespace StdLib;

bool NativeConsole::IsSupported()
{
	return false;
}

NativeConsole::~NativeConsole()
{}

NativeConsole::NativeConsole(bool isAttachToExisting, Error<> *error)
{
	if (error)
	{
		*error = DefaultError::Unsupported();
	}
}

Error<> NativeConsole::Open(bool isAttachToExisting)
{
	return DefaultError::Unsupported();
}

void NativeConsole::Close()
{}

bool NativeConsole::IsOpen() const
{
	return false;
}

NativeConsole &NativeConsole::CursorPosition(std::optional<ui32> x, std::optional<ui32> y)
{
	HARDBREAK;
	return *this;
}

std::pair<ui32, ui32> NativeConsole::CursorPosition() const
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::Write(pathStringView text)
{
	HARDBREAK;
	return *this;
}

NativeConsole &NativeConsole::WriteASCII(std::string_view text)
{
	HARDBREAK;
	return *this;
}

NativeConsole &NativeConsole::Title(pathStringView title)
{
	HARDBREAK;
	return *this;
}

NativeConsole &NativeConsole::TitleASCII(std::string_view title)
{
	HARDBREAK;
	return *this;
}

pathString NativeConsole::Title() const
{
	HARDBREAK;
	return {};
}

std::string NativeConsole::TitleASCII() const
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::Colors(Color textColor, Color backgroundColor)
{
	HARDBREAK;
	return *this;
}

auto NativeConsole::Colors() const -> std::pair<Color, Color>
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::Position(std::optional<i32> x, std::optional<i32> y)
{
	HARDBREAK;
	return *this;
}

std::pair<i32, i32> NativeConsole::Position() const
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::Size(std::optional<ui32> width, std::optional<ui32> height)
{
	HARDBREAK;
	return *this;
}

std::pair<ui32, ui32> NativeConsole::Size() const
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::BufferSize(std::optional<ui32> charactersPerLine, std::optional<ui32> lines)
{
	HARDBREAK;
	return *this;
}

std::pair<ui32, ui32> NativeConsole::BufferSize() const
{
	HARDBREAK;
	return {};
}

NativeConsole &NativeConsole::SnapToBufferSize(bool isSnapByX, bool isSnapByY)
{
	HARDBREAK;
	return *this;
}

NativeConsole &NativeConsole::SnapToWindowSize(bool isSnapByX, bool isSnapByY)
{
	HARDBREAK;
	return *this;
}

NativeConsole &NativeConsole::Modes(ModeValues::ModeValue modes)
{
	HARDBREAK;
	return *this;
}

auto NativeConsole::Modes() const -> ModeValues::ModeValue
{
	HARDBREAK;
	return {};
}