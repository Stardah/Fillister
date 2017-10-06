#pragma once
#include <LiquidCrystal.h>

enum Menus { Select, InpAmt, InpPause, Run, Service };

class Menu
{
public:
	Menu(const LiquidCrystal& lcdInit);
	~Menu() = default;

	void UpdateValues(long leng, long amt, long width); //
	void SetMenuMode(long newMenu);		// Switches mode
	void ApplyInpAmt(long& lot, long& seria);	// Save input values (lot, series)
	void ApplyInpPause(long& pause);			// Save input value pause
	void ApplySettings(long& leng, long& amt, long& width); // Memorize settings
	void DrawMenu();
	void DrawServiceScreen(long inputs[12], long encoderCounter);
	void DrawRunScreen(long length, long parts); // To display current length and parts
	void Input(char word);
	void DelLast();
	void Up();
	void Down();
	Menus getMenu()
	{
		return menuMode;
	}
private:
	static const size_t maxX = 5;
	static const size_t maxY = 9;
	Menus menuMode = Menus::Select;
	LiquidCrystal lcd;
	String items[maxX][maxY];
	long curX = 0;
	long curY = 0;
	bool upside = true;
	long lotCount;
	long seriesCount;
	long pauseShift;
	long length = 0;
	long parts = 0;
};