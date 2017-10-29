#pragma once
#include <LiquidCrystal.h>

enum Menus { Select, InpAmt, InpPause, Run, Service, Count };

class Menu
{
public:
	Menu(const LiquidCrystal& lcdInit);
	~Menu() = default;

	//void UpdateValues(long lot, long series, long pause); // lot, series. pauseShift
	void SetPerfomance(long perfomance);
	void SetParameters(long lotMax, long seriesMax, long pause) 
	{
		lotCount = lotMax;
		seriesCount = seriesMax;
		pauseShift = pause;
	};
	void SetMenuMode(long newMenu);		// Switches mode
	void ApplyInpAmt(long& lot, long& series);	// Save input values (lot, series)
	void ApplyInpPause(int& pause);			// Save input value pause
	void DrawMenu();
	void DrawServiceScreen(long inputs[3], long encoderValue, long probeg, long pp);
	void DrawRunScreen(long lot, long series); // To display current length and parts
	void DrawCounterScreen(long curLot);
	void Input(char word);
	void DelLast();
	void Up();
	void Down();
	Menus getMenu()
	{
		return menuMode;
	}
private:
	static const size_t maxX = 6;
	static const size_t maxY = 9;
	Menus menuMode = Menus::Select;
	LiquidCrystal lcd;
	String items[maxX][maxY];
	long curX = 0;
	long curY = 0;
	bool upside = true;
	long lotCount = 0;
	long seriesCount = 0;
	long pauseShift;
};