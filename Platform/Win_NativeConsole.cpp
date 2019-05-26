#include "_PreHeader.hpp"
#include "NativeConsole.hpp"
#include <TlHelp32.h>

using namespace StdLib;

namespace
{
	const WORD TextColorMap[] =
	{
		/*Black*/ 0,
		/*White*/ FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
		/*Blue*/ FOREGROUND_BLUE,
		/*Green*/ FOREGROUND_GREEN,
		/*Red*/ FOREGROUND_RED,
		/*Yellow*/ FOREGROUND_RED | FOREGROUND_GREEN,
		/*Magenta*/ FOREGROUND_RED | FOREGROUND_BLUE,
		/*Cyan*/ FOREGROUND_GREEN | FOREGROUND_BLUE,
		/*LightWhite*/ FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		/*LightBlue*/ FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		/*LightGreen*/ FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		/*LightRed*/ FOREGROUND_RED | FOREGROUND_INTENSITY,
		/*LightYellow*/ FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		/*LightMagenta*/ FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		/*LightCyan*/ FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	const WORD BackgroundColorMap[] =
	{
		/*Black*/ 0,
		/*White*/ BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED,
		/*Blue*/ BACKGROUND_BLUE,
		/*Green*/ BACKGROUND_GREEN,
		/*Red*/ BACKGROUND_RED,
		/*Yellow*/ BACKGROUND_RED | BACKGROUND_GREEN,
		/*Magenta*/ BACKGROUND_RED | BACKGROUND_BLUE,
		/*Cyan*/ BACKGROUND_GREEN | BACKGROUND_BLUE,
		/*LightWhite*/ BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
		/*LightBlue*/ BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		/*LightGreen*/ BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		/*LightRed*/ BACKGROUND_RED | BACKGROUND_INTENSITY,
		/*LightYellow*/ BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		/*LightMagenta*/ BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		/*LightCyan*/ BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	};
}

static CONSOLE_FONT_INFO GetFontInfo(HANDLE handle);
static CONSOLE_SCREEN_BUFFER_INFO GetInfo(HANDLE handle);
//static void SetInfo(HANDLE handle, CONSOLE_SCREEN_BUFFER_INFO &info);
static void SetColors(HANDLE handle, std::optional<NativeConsole::Color> textColor, std::optional<NativeConsole::Color> backgroundColor);

bool NativeConsole::IsSupported()
{
	return true;
}

NativeConsole::~NativeConsole()
{
	Close();
}

NativeConsole::NativeConsole(bool isAttachToExisting, Error<> *error)
{
	Error<> result = Open(isAttachToExisting);
	if (error)
	{
		*error = result;
	}
}

NativeConsole::NativeConsole(NativeConsole &&source) noexcept : _inputHandle(source._inputHandle), _outputHandle(source._outputHandle), _isAttached(source._isAttached)
{
	ASSUME(this != &source);
	source._inputHandle = consoleHandle_undefined;
	source._outputHandle = consoleHandle_undefined;
}

NativeConsole &NativeConsole::operator = (NativeConsole &&source) noexcept
{
	ASSUME(this != &source);
	_inputHandle = source._inputHandle;
	source._inputHandle = consoleHandle_undefined;
	_outputHandle = source._outputHandle;
	source._outputHandle = consoleHandle_undefined;
	_isAttached = source._isAttached;
	return *this;
}

Error<> NativeConsole::Open(bool isAttachToExisting)
{
	Close();

	if (isAttachToExisting == false)
	{
		BOOL result = AllocConsole();
		if (result != TRUE)
		{
			return DefaultError::AlreadyExists("AllocConsole failed, seems like a console already exists, but isAttachToExisting is false");
		}

		FILE *file = freopen("CONIN$", "r", stdin);
		file = freopen("CONOUT$", "w", stdout);
		file = freopen("CONOUT$", "w", stderr);
		UNREFERENCED_PARAMETER(file);
	}

	_isAttached = isAttachToExisting;
	_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	_inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	return DefaultError::Ok();
}

void NativeConsole::Close()
{
	if (_outputHandle != INVALID_HANDLE_VALUE && _isAttached == false)
	{
		BOOL result = FreeConsole();
		ASSUME(result);
	}
	_outputHandle = INVALID_HANDLE_VALUE;
	_inputHandle = INVALID_HANDLE_VALUE;
}

bool NativeConsole::IsOpen() const
{
	return _outputHandle != INVALID_HANDLE_VALUE;
}

NativeConsole &NativeConsole::CursorPosition(std::optional<ui32> x, std::optional<ui32> y)
{
	ASSUME(IsOpen());
	if (x == std::nullopt || y == std::nullopt)
	{
		auto info = GetInfo(_outputHandle);
		if (x == std::nullopt)
		{
			x = (ui32)info.dwCursorPosition.X;
		}
		if (y == std::nullopt)
		{
			y = (ui32)info.dwCursorPosition.Y;
		}
	}
	COORD coord;
	coord.X = (SHORT)*x;
	coord.Y = (SHORT)*y;
	BOOL result = SetConsoleCursorPosition(_outputHandle, coord);
	ASSUME(result);
	return *this;
}

std::pair<ui32, ui32> NativeConsole::CursorPosition() const
{
	ASSUME(IsOpen());
	auto info = GetInfo(_outputHandle);
	return {(ui32)info.dwCursorPosition.X, (ui32)info.dwCursorPosition.Y};
}

NativeConsole &NativeConsole::Write(pathStringView text, ui32 *printed)
{
	ASSUME(IsOpen());

	DWORD written;
	BOOL result = WriteConsoleW(_outputHandle, text.data(), (DWORD)text.size(), &written, nullptr);
	ASSUME(result);
	if (printed)
	{
		*printed = written;
	}

	return *this;
}

NativeConsole &NativeConsole::WriteASCII(std::string_view text, ui32 *printed)
{
	ASSUME(IsOpen());

	DWORD written;
	BOOL result = WriteConsoleA(_outputHandle, text.data(), (DWORD)text.size(), &written, nullptr);
	ASSUME(result);
	if (printed)
	{
		*printed = written;
	}

	return *this;
}

NativeConsole &NativeConsole::Title(pathStringView title)
{
	ASSUME(IsOpen());
	pathString temp;
	if (title.empty() || title.back() != L'\0')
	{
		temp = title;
		title = temp;
	}
	BOOL result = SetConsoleTitleW(title.data());
	ASSUME(result);
	return *this;
}

NativeConsole &NativeConsole::TitleASCII(std::string_view title)
{
	ASSUME(IsOpen());
	std::string temp;
	if (title.empty() || title.back() != '\0')
	{
		temp = title;
		title = temp;
	}
	BOOL result = SetConsoleTitleA(title.data());
	ASSUME(result);
	return *this;
}

pathString NativeConsole::Title() const
{
	ASSUME(IsOpen());
	wchar_t temp[MAX_PATH];
	DWORD result = GetConsoleTitleW(temp, MAX_PATH);
	ASSUME(result);
	return {temp, result};
}

std::string NativeConsole::TitleASCII() const
{
	ASSUME(IsOpen());
	char temp[MAX_PATH];
	DWORD result = GetConsoleTitleA(temp, MAX_PATH);
	ASSUME(result);
	return {temp, result};
}

NativeConsole &NativeConsole::Colors(Color textColor, Color backgroundColor)
{
	SetColors(_outputHandle, textColor, backgroundColor);
	return *this;
}

auto NativeConsole::Colors() const -> std::pair<Color, Color>
{
	ASSUME(IsOpen());

	auto info = GetInfo(_outputHandle);

	WORD textColor = info.wAttributes & 0xFFF0;
	WORD backgroundColor = info.wAttributes & 0xFF0F;

	Color left, right;

	for (uiw index = 0; ; ++index)
	{
		ASSUME(index < CountOf(TextColorMap));
		if (TextColorMap[index] == textColor)
		{
			left = (Color)index;
			break;
		}
	}
	for (uiw index = 0; ; ++index)
	{
		ASSUME(index < CountOf(BackgroundColorMap));
		if (BackgroundColorMap[index] == backgroundColor)
		{
			right = (Color)index;
			break;
		}
	}

	return {left, right};
}

NativeConsole &NativeConsole::Position(std::optional<i32> x, std::optional<i32> y)
{
	ASSUME(IsOpen());
	HWND window = GetConsoleWindow();
	ASSUME(window);
	RECT rect;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	if (x == std::nullopt)
	{
		x = (i32)rect.left;
	}
	if (y == std::nullopt)
	{
		y = (i32)rect.top;
	}
	result = MoveWindow(window, *x, *y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ASSUME(result);
	return *this;
}

std::pair<i32, i32> NativeConsole::Position() const
{
	ASSUME(IsOpen());
	HWND window = GetConsoleWindow();
	ASSUME(window);
	RECT rect;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	return {(i32)rect.left, (i32)rect.top};
}

NativeConsole &NativeConsole::Size(std::optional<ui32> width, std::optional<ui32> height)
{
	ASSUME(IsOpen());

	auto info = GetInfo(_outputHandle);
	HWND window = GetConsoleWindow();
	ASSUME(window);
	RECT rect;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	if (width == std::nullopt)
	{
		width = (ui32)(rect.right - rect.left);
	}
	if (height == std::nullopt)
	{
		height = (ui32)(rect.bottom - rect.top);
	}

	// increase screen buffer size if it isn't big enough for the new window size
	RECT client;
	result = GetClientRect(window, &client);
	ASSUME(result);
	i32 xDelta = (rect.right - rect.left) - (client.right - client.left);
	i32 yDelta = (rect.bottom - rect.top) - (client.bottom - client.top);
	i32 newClientWidth = (i32)*width - xDelta;
	i32 newClientHeight = (i32)*height - yDelta;
	CONSOLE_FONT_INFO fontInfo;
	result = GetCurrentConsoleFont(_outputHandle, FALSE, &fontInfo);
	ASSUME(result);
	i32 currentBufferWidthPixels = info.dwSize.X * fontInfo.dwFontSize.X;
	i32 currentBufferHeightPixels = info.dwSize.Y * fontInfo.dwFontSize.Y;
	if (newClientWidth > currentBufferWidthPixels || newClientHeight > currentBufferHeightPixels)
	{
		currentBufferWidthPixels = std::max(currentBufferWidthPixels, newClientWidth);
		currentBufferHeightPixels = std::max(currentBufferHeightPixels, newClientHeight);

		i32 currentBufferWidth = currentBufferWidthPixels / fontInfo.dwFontSize.X;
		i32 currentBufferHeight = currentBufferHeightPixels / fontInfo.dwFontSize.Y;

		COORD bufSize = {(SHORT)currentBufferWidth, (SHORT)currentBufferHeight};
		result = SetConsoleScreenBufferSize(_outputHandle, bufSize);
		ASSUME(result);
	}

	result = MoveWindow(window, rect.left, rect.top, (i32)*width, (i32)*height, TRUE);
	ASSUME(result);

	return *this;
}

std::pair<ui32, ui32> NativeConsole::Size() const
{
	ASSUME(IsOpen());
	HWND window = GetConsoleWindow();
	ASSUME(window);
	RECT rect;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	return {(ui32)(rect.right - rect.left), (ui32)(rect.bottom - rect.top)};
}

NativeConsole &NativeConsole::BufferSize(std::optional<ui32> charactersPerLine, std::optional<ui32> lines)
{
	ASSUME(IsOpen());
	if (charactersPerLine == std::nullopt || lines == std::nullopt)
	{
		auto info = GetInfo(_outputHandle);
		if (charactersPerLine == std::nullopt)
		{
			charactersPerLine = (ui32)info.dwSize.X;
		}
		if (lines == std::nullopt)
		{
			lines = (ui32)info.dwSize.Y;
		}
	}
	COORD bufSize = {(SHORT)*charactersPerLine, (SHORT)*lines};
	BOOL result = SetConsoleScreenBufferSize(_outputHandle, bufSize);
	ASSUME(result);
	return *this;
}

NativeConsole &NativeConsole::SnapToBufferSize(bool isSnapByX, bool isSnapByY)
{
	ASSUME(IsOpen());

	HWND window = GetConsoleWindow();
	RECT rect, client;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	result = GetClientRect(window, &client);
	ASSUME(result);
	i32 xDelta = (rect.right - rect.left) - (client.right - client.left);
	i32 yDelta = (rect.bottom - rect.top) - (client.bottom - client.top);

	auto info = GetInfo(_outputHandle);
	auto fontInfo = GetFontInfo(_outputHandle);
	i32 bufWidthPixels = info.dwSize.X * fontInfo.dwFontSize.X;
	i32 bufHeightPixels = info.dwSize.Y * fontInfo.dwFontSize.Y;

	i32 newCientWidth = isSnapByX ? bufWidthPixels : (client.right - client.left);
	i32 newClientHeight = isSnapByY ? bufHeightPixels : (client.bottom - client.top);

	i32 newWindowWidth = newCientWidth + xDelta;
	i32 newWindowHeight = newClientHeight + yDelta;

	result = MoveWindow(window, rect.left, rect.top, newWindowWidth, newWindowHeight, TRUE);
	ASSUME(result);

	return *this;
}

NativeConsole &NativeConsole::SnapToWindowSize(bool isSnapByX, bool isSnapByY)
{
	ASSUME(IsOpen());

	HWND window = GetConsoleWindow();
	RECT rect, client;
	BOOL result = GetWindowRect(window, &rect);
	ASSUME(result);
	result = GetClientRect(window, &client);
	ASSUME(result);
	i32 xDelta = (rect.right - rect.left) - (client.right - client.left);
	i32 yDelta = (rect.bottom - rect.top) - (client.bottom - client.top);

	auto info = GetInfo(_outputHandle);
	auto fontInfo = GetFontInfo(_outputHandle);
	i32 bufWidthPixels = info.dwSize.X * fontInfo.dwFontSize.X;
	i32 bufHeightPixels = info.dwSize.Y * fontInfo.dwFontSize.Y;

	i32 newBufferWidthPixels = isSnapByX ? (client.right - client.left) : bufWidthPixels;
	i32 newBufferHeightPixels = isSnapByY ? (client.bottom - client.top) : bufHeightPixels;

	i32 newBufferWidth = newBufferWidthPixels / fontInfo.dwFontSize.X;
	i32 newBufferHeight = newBufferHeightPixels / fontInfo.dwFontSize.Y;

	COORD bufSize = {(SHORT)newBufferWidth, (SHORT)newBufferHeight};
	result = SetConsoleScreenBufferSize(_outputHandle, bufSize);
	ASSUME(result);

	return *this;
}

std::pair<ui32, ui32> NativeConsole::BufferSize() const
{
	ASSUME(IsOpen());
	auto info = GetInfo(_outputHandle);
	return {(ui32)info.dwSize.X, (ui32)info.dwSize.Y};
}

NativeConsole &NativeConsole::Modes(ModeValues::ModeValue modes)
{
	DWORD mode = 0;
	if (modes.Contains(ModeValues::EnableMouseSelection))
	{
		mode |= ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS;
	}
	if (modes.Contains(ModeValues::ProcessInput))
	{
		mode |= ENABLE_PROCESSED_INPUT;
	}
	BOOL result = SetConsoleMode(_inputHandle, mode);
	ASSUME(result);
	return *this;
}

auto NativeConsole::Modes() const -> ModeValues::ModeValue
{
	ModeValues::ModeValue collected;
	DWORD mode;
	BOOL result = GetConsoleMode(_inputHandle, &mode);
	ASSUME(result);
	if (mode & (ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS))
	{
		collected.Add(ModeValues::EnableMouseSelection);
	}
	if (mode & ENABLE_PROCESSED_INPUT)
	{
		collected.Add(ModeValues::ProcessInput);
	}
	return collected;
}

CONSOLE_FONT_INFO GetFontInfo(HANDLE handle)
{
	ASSUME(handle != INVALID_HANDLE_VALUE);
	CONSOLE_FONT_INFO font;
	BOOL result = GetCurrentConsoleFont(handle, FALSE, &font);
	ASSUME(result);
	return font;
}

CONSOLE_SCREEN_BUFFER_INFO GetInfo(HANDLE handle)
{
	ASSUME(handle != INVALID_HANDLE_VALUE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	BOOL result = GetConsoleScreenBufferInfo(handle, &info);
	ASSUME(result);
	return info;
}

//void SetInfo(HANDLE handle, CONSOLE_SCREEN_BUFFER_INFO &info)
//{
//	ASSUME(handle != INVALID_HANDLE_VALUE);
//	BOOL result = SetConsoleScreenBufferInfo(handle, &info);
//	ASSUME(result);
//}

void SetColors(HANDLE handle, std::optional<NativeConsole::Color> textColor, std::optional<NativeConsole::Color> backgroundColor)
{
	ASSUME(handle != INVALID_HANDLE_VALUE);

	auto info = GetInfo(handle);
	WORD mask = info.wAttributes;

	if (textColor)
	{
		mask &= 0xFFF0;
		mask |= TextColorMap[(uiw)*textColor];
	}
	if (backgroundColor)
	{
		mask &= 0xFF0F;
		mask |= BackgroundColorMap[(uiw)*backgroundColor];
	}

	DWORD result = SetConsoleTextAttribute(handle, mask);
	ASSUME(result);
}