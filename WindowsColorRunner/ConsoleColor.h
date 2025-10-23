#ifndef CONSOLECOLOR_H
#define CONSOLECOLOR_H

#include <windows.h>

namespace TddConsole
{
class ConsoleColor
{
    HANDLE  m_screen;
    WORD m_wOriginalAttributes;
public:
    enum Color
    {
        Black        = 0,
        Blue         = 1,
        Green        = 2,
        Cyan         = 3,
        Red          = 4,
        Magenta      = 5,
        Brown        = 6,
        LightGrey    = 7,
        DarkGrey     = 8,
        LightBlue    = 9,
        LightGreen   = 10,
        LightCyan    = 11,
        LightRed     = 12,
        LightMagenta = 13,
        Yellow       = 14,
        White        = 15,
		Blink        = 128,
    };

    ConsoleColor()
		: m_screen(::GetStdHandle(STD_OUTPUT_HANDLE))
		, m_wOriginalAttributes(GetOriginalAttributes(m_screen))
    {}
	virtual ~ConsoleColor() { Reset(); }

    void SetFontColor(WORD color)
    {
		color &= 0x8F;
		WORD wAttribute = (m_wOriginalAttributes & 0xF0) | color;
		::SetConsoleTextAttribute(m_screen, wAttribute);
    }

    void SetFontAndBackgroundColors(WORD fontColor, WORD backgroundColor)
    {
		fontColor &= 0x8F;
		backgroundColor &= 0xF;
		WORD wAttribute = (backgroundColor << 4) | fontColor;
		::SetConsoleTextAttribute(m_screen, wAttribute);
    }
    
    void Reset(void)
    {
		::SetConsoleTextAttribute(m_screen, m_wOriginalAttributes);
    }

private:
	static WORD GetOriginalAttributes(HANDLE handle)
	{
		CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = {0};
		::GetConsoleScreenBufferInfo(handle, &consoleScreenBufferInfo);
        return consoleScreenBufferInfo.wAttributes;
	}
};
}
#endif