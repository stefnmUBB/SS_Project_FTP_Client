#pragma once

#include <iostream>
#include <windows.h>

namespace Utils
{
	struct Color
	{
		int code;

		Color(int code) : code(code) { }

		static Color Red() { return Color{ FOREGROUND_INTENSITY | FOREGROUND_RED }; }
		static Color Blue() { return Color{ FOREGROUND_INTENSITY | FOREGROUND_BLUE }; }
		static Color Yellow() { return Color{ FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN }; }
		static Color White() { return Color{ FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE }; }

		friend std::ostream& operator << (std::ostream& o, const Color& color);
	};

	std::ostream& operator << (std::ostream& o, const Color& color);	

	inline static constexpr char nibble2chr(int nibble)
	{
		nibble &= 0xF;
		if (nibble < 10) return '0' + nibble;
		return 'A' + nibble;
	}

}