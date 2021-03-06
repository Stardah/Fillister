#include "Menu.h"
#include <locale.h>

#define A String('A')
#define B String(char(160))
#define V String('B')
#define G String(char(161))
#define D String(char(224))
#define E String('E')
#define Yo String(char(162))
#define Zh String(char(163))
#define Z String(char(164))
#define I String(char(165))
#define Ii String(char(166))
#define K String('K')
#define L String(char(167))
#define M String('M')
#define N String('H')
#define O String('O')
#define P String(char(168))
#define R String('P')
#define S String('C')
#define T String('T')
#define Y String(char(169))
#define F String(char(170))
#define X String('X')
#define Tz String(char(229))
#define Ch String(char(171))
#define Sh String(char(172))
#define Shi String(char(226))
#define tverdiy String(char(173))
#define Ui String(char(174))
#define myagkiy String('b')
#define Ee String(char(175))
#define Yu String(char(176))
#define Ya String(char(177))

#define a String('a')
#define b String(char(178))
#define v String(char(179))
#define g String(char(180))
#define d String(char(227))
#define e String('e')
#define yo String(char(181))
#define zh String(char(182))
#define z String(char(183))
#define si String(char(184))
#define ii String(char(185))
#define k String(char(186))
#define l String(char(187))
#define m String(char(188))
#define n String(char(189))
#define o String('o')
#define p String(char(190))
#define r String('p')
#define s String('c')
#define t String(char(191))
#define y String('y')
#define f String(char(228))
#define x String('x')
#define tz String(char(229))
#define ch String(char(192))
#define sh String(char(193))
#define shi String(char(230))
#define stverdiy String(char(194))
#define ui String(char(195))
#define smyagkiy String(char(196))
#define ee String(char(197))
#define u String(char(198))
#define ya String(char(199))

String vsego = V + s + e + g + o + ": ";
String seriya = S + e + r + si + ya + ": ";
String iz = I + z;
String lagInpText1 = s + e + k + y + n + d + ": ";
String serviceText1 = P + r + o + b + e + g;
String ppText = " " + l + si + s + t + o + v + "/" + m + si + n;
const int inpGapLot = 7;
const int inpGapSeries = 7;
const int inpGapPause = 8;
const int inpLotLetters = 5;
const int inpPauseLetters = 3;
const int selectM = 0;
const int amtInpM = 1;
const int lagInpM = 2;
const int runM = 3;
const int serviceM = 4;
const int countM = 5;
Menu::Menu(const LiquidCrystal& lcdInit) :
	lcd(lcdInit)
{
	lcd.begin(16, 2);
	for (size_t i = 0; i < maxX; i++)
	{
		for (size_t j = 0; j < maxY; j++)
		{
			if (j != maxY - 1)
				items[i][j] = "               ";
			else
				items[i][j] = "|______________|";
		}
	}

	// curX = 0 - choose mode - main screen 
	items[selectM][0] = R + e + zh + si + m + ": 1-" + r + y + ch + n;
	items[selectM][1] = "2-" + s + e + r + si + ya + " 3-" + s + d + v + si + g;

	// curX = 1 - input lot, series
	items[amtInpM][0] = vsego + "100";
	items[amtInpM][1] = seriya + "20";

	// curX = 2 - input cooldown
	items[lagInpM][0] = P + a + y + z + a + " " + s + d + v + si + g + a;
	items[lagInpM][1] = lagInpText1 + "4";

	// curX = 3 - run screen
	items[runM][0] = vsego;
	items[runM][1] = seriya;
	items[runM][2] = "0" + ppText;

	// curX = 4 - service screen
	items[serviceM][0] = serviceText1 + " = ";

	// curX = 5 - count screen

	lcd.print("  " + Z + a + g + r + y + z + k + a + "...");
}

void Menu::SetPerfomance(long perfomance)
{
	items[runM][2] = String(perfomance)+ ppText;
}

void Menu::DrawMenu()
{
	lcd.clear();
	if ((menuMode == InpAmt || menuMode == InpPause) && upside) // cursor after selected line
	{
		lcd.setCursor(0, 1);
		lcd.print(items[curX][curY + 1]);
		lcd.setCursor(0, 0);
		lcd.print(items[curX][curY]);
	}
	else
	{
		lcd.setCursor(0, 0);
		lcd.print(items[curX][curY]);
		lcd.setCursor(0, 1);
		lcd.print(items[curX][curY + 1]);
	}
}

///
// Draw service menu
///
String inp[3];
void Menu::DrawServiceScreen(long inputs[3], long encoderCounter, long probeg, long pp)
{
	for (long i = 0; i< 3; i++)
		if (inputs[i] == 1) inp[i] = "1";
			else inp[i] = "0";
		
		items[serviceM][0] = serviceText1 + " = " + String(probeg);
		items[serviceM][1] = String(pp) + ppText;
		items[serviceM][2] = "MoA = " + inp[0] + " MoB = " + inp[1];
		items[serviceM][3] = "Sound = " + inp[2];
		items[serviceM][4] = D + a + t + ch + si + k + " = " + String(encoderCounter);

		if (upside)
		{
			lcd.setCursor(0, 1);
			lcd.print(items[serviceM][curY + 1]);
			lcd.setCursor(0, 0);
			lcd.print(items[serviceM][curY]);
		}
		else
		{
			lcd.setCursor(0, 0);
			lcd.print(items[serviceM][curY]);
			lcd.setCursor(0, 1);
			lcd.print(items[serviceM][curY + 1]);
		}
}

///
// Draw run mode menu
///
void Menu::DrawRunScreen(long curLot, long curSeries)
{
	items[runM][0] = vsego + String(curLot) + "/" + String(lotCount);
	items[runM][1] = seriya + String(curSeries) + "/" + String(seriesCount);
}

void Menu::DrawCounterScreen(long curLot)
{
	items[countM][0] = vsego + String(curLot);
	items[countM][1] = " ";
}

///
// Switch to chosen menu mode
///
void Menu::SetMenuMode(long newMenu)
{
	upside = true;
	menuMode = Menus(newMenu);
	curY = 0;
	switch (newMenu)
	{
	case InpAmt:
		curX = amtInpM;
		break;
	case InpPause:
		upside = false;
		curX = lagInpM;
		break;
	case Service:
		curX = serviceM;
		break;
	case Select:
		curX = selectM;
		break;
	case Run:
		curX = runM;
		break;
	case Count:
		curX = countM;
		break;
	default:
		break;
	}
}

///
// Apply input for lot and series 
///
void Menu::ApplyInpAmt(long &lot, long &series)
{
	lot = items[amtInpM][0].substring(inpGapLot).toInt();
	series = items[amtInpM][1].substring(inpGapSeries).toInt();
	lotCount = lot;
	seriesCount = series;
}

///
// Apply input for pause
///
void Menu::ApplyInpPause(int & pause)
{
	pause = items[lagInpM][1].substring(inpGapPause).toInt();
	pauseShift = pause;
}

///
// Input char
///
void Menu::Input(char cha)
{
	if (menuMode == Menus::InpAmt)
	{
		if (upside && items[curX][curY].length() < (inpGapLot + inpLotLetters))  // "dlin: "  5 numbers
			items[curX][curY] += cha;
		if (!upside && items[curX][curY + 1].length() < (inpGapSeries + inpLotLetters))  // "kol: " 5 numbers
			items[curX][curY + 1] += cha;
	}
	else if (menuMode == Menus::InpPause)
	{
		if (items[curX][curY + 1].length() < (inpGapPause + inpPauseLetters))  // "kol: " 5 numbers
			items[curX][curY + 1] += cha;
	}
}

///
// Delete last symbol
///
void Menu::DelLast()
{
	if (menuMode == Menus::InpAmt)
	{
		if (upside && items[curX][curY].length() > inpGapLot) 
		{
			items[curX][curY] = items[curX][curY].substring(0, items[curX][curY].length() - 1);
			lcd.clear();
		}
		if (!upside && items[curX][curY + 1].length() > inpGapSeries) 
		{
			items[curX][curY + 1] = items[curX][curY + 1].substring(0, items[curX][curY + 1].length() - 1);
			lcd.clear();
		}
	}
	else if (menuMode == Menus::InpPause)
	{
		if (items[curX][curY + 1].length() > inpGapPause)
		{
			items[curX][curY + 1] = items[curX][curY + 1].substring(0, items[curX][curY + 1].length() - 1);
			lcd.clear();
		}
	}
}

void Menu::Down()
{
	lcd.clear();
	if (menuMode == Menus::Service && curY < maxY - 3) ++curY;
	else if (menuMode == Menus::Run && curY < 2) ++curY;
	else if (upside) upside = !upside;
}

void Menu::Up()
{
	lcd.clear();
	if (menuMode == Menus::Service && curY > 0) --curY;
	else if (menuMode == Menus::Run && curY > 0) --curY;
	else if (!upside) upside = !upside;
}


