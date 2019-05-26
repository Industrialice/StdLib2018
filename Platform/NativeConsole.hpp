#pragma once

WARNING_PUSH
WARNING_DISABLE_ATTRIBUTE_IS_NOT_RECOGNIZED

namespace StdLib
{
	class NativeConsole
	{
	public:
		enum class Color : ui8
		{
			Black, White, Blue, Green,
			Red, Yellow, Magenta, Cyan,
			LightWhite, LightBlue, LightGreen, 
			LightRed, LightYellow, LightMagenta, 
			LightCyan
		};

		struct ModeValues
		{
			static constexpr struct ModeValue : EnumCombinable<ModeValue, ui8, true>
			{} Default = ModeValue::Create(0),
				EnableMouseSelection = ModeValue::Create(Funcs::BitPos(0)),
				ProcessInput = ModeValue::Create(Funcs::BitPos(1)); /* processing key combinations like Control + C */
		};

	private:
		consoleHandle _outputHandle = consoleHandle_undefined;
		consoleHandle _inputHandle = consoleHandle_undefined;
		bool _isAttached{};

	public:

		static bool IsSupported();

		~NativeConsole();
		NativeConsole() = default;
		NativeConsole(bool isAttachToExisting, Error<> *error = nullptr);
		NativeConsole(NativeConsole &&source) noexcept;
		NativeConsole &operator = (NativeConsole &&source) noexcept;
		Error<> Open(bool isAttachToExisting);
		void Close();
		[[nodiscard]] bool IsOpen() const;
		[[must_be_open]] NativeConsole &CursorPosition(std::optional<ui32> x, std::optional<ui32> y);
		[[must_be_open]] [[nodiscard]] std::pair<ui32, ui32> CursorPosition() const;
		[[must_be_open]] NativeConsole &Write(pathStringView text, ui32 *printed = nullptr);
		[[must_be_open]] NativeConsole &WriteASCII(std::string_view text, ui32 *printed = nullptr);
		[[must_be_open]] NativeConsole &Title(pathStringView title);
		[[must_be_open]] NativeConsole &TitleASCII(std::string_view title);
		[[nodiscard]] pathString Title() const;
		[[nodiscard]] std::string TitleASCII() const;
		[[must_be_open]] NativeConsole &Colors(Color textColor, Color backgroundColor);
		[[must_be_open]] [[nodiscard]] std::pair<Color, Color> Colors() const;
		[[must_be_open]] NativeConsole &Position(std::optional<i32> x, std::optional<i32> y);
		[[must_be_open]] [[nodiscard]] std::pair<i32, i32> Position() const;
		[[must_be_open]] NativeConsole &Size(std::optional<ui32> width, std::optional<ui32> height);
		[[must_be_open]] [[nodiscard]] std::pair<ui32, ui32> Size() const;
		[[must_be_open]] NativeConsole &BufferSize(std::optional<ui32> charactersPerLine, std::optional<ui32> lines);
		[[must_be_open]] [[nodiscard]] std::pair<ui32, ui32> BufferSize() const;
		[[must_be_open]] NativeConsole &SnapToBufferSize(bool isSnapByX, bool isSnapByY = false); // resizes the window to fit the screen buffers
		[[must_be_open]] NativeConsole &SnapToWindowSize(bool isSnapByX, bool isSnapByY = false); // resizes the screen buffers to fit into the window
		[[must_be_open]] NativeConsole &Modes(ModeValues::ModeValue modes);
		[[must_be_open]] [[nodiscard]] ModeValues::ModeValue Modes() const;
		
		explicit operator bool() const
		{
			return IsOpen();
		}
	};
}

WARNING_POP