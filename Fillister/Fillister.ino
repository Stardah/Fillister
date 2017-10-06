/*
 Name:		Fillister.ino
 Created:	06.10.2017 15:36:06
 Author:	Adrax
*/

#include <Key.h>
#include <Keypad.h>
#include "Menu.h" 
#include "ControlPins.h"
#include "ProgStruct.cpp"
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Display
LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

// Keypad
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] =
{ // Array buttons-chars
	{ '1','2','3','A' },
	{ '4','5','6','B' },
	{ '7','8','9','C' },
	{ '*','0','#','D' }
};
byte rowPins[ROWS] = { 66, 67, 68, 69 }; // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 62, 63, 64, 65 }; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// End keypad

// External classes
Menu menu(lcd);	// Menu update
ControlPins controlPins(20); // Pins state update, error = 20

PROG programs; // Actually don't need this
Menus cash = Menus::Select; // Save previous menu
bool progRun = false; // Access to write program
bool serviceOn = false; // Service mode on
bool isInputAmt = false;
volatile long encoderValue = 0; // Encoder mm counter

								  // Settings
long eps = 11;
long epsOld = 11;
long coolDown = 12;
long coolDownOld = 12;
//volatile bool encoderB = false;
long kostyl = 0; // TODO replace it with timer
long programMode = 0;
long lotMax = 100;
long seriesMax = 20;
long pauseShift = 10;

void Addprog(long leng, long amt)
{
	programs.leng = leng;
	programs.amt = amt;
}

// addr		var
//	0		eps < 100
//  1       coolDown/256 
//  2       coolDown%256
void setup()
{
	//read settings
	/*eps = EEPROM.read(0);
	coolDown = EEPROM.read(1) * 256 + EEPROM.read(2);
	nozh = EEPROM.read(3);

	epsOld = eps;
	coolDownOld = coolDown;
	nozhOld = nozh;
	*/
	controlPins.SetCoolDown(coolDown);
	
	//setup
	lcd.begin(16, 2);

	 // ??? recover previous prog
	menu.UpdateValues(lotMax, seriesMax, pauseShift);
	menu.DrawMenu();

	attachInterrupt(2, EncoderChange, FALLING);
}

void loop()
{
	// Update Input
	//
	char key = keypad.getKey();
	// Update Gear
	//

	kostyl++;
	if (kostyl > 2000)
	{
		if (progRun)
		{
			menu.DrawRunScreen(controlPins.GetLot(encoderValue), controlPins.GetSeries(encoderValue)); // Display current values
			menu.DrawMenu();
			kostyl = 0;
			if (controlPins.GetLot(encoderValue) >= lotMax)
			{
				progRun = false;
				lcd.cursor();
				lcd.blink();
				controlPins.Reset();
				menu.SetMenuMode(Menus::InpAmt);
				menu.DrawMenu();
			}
		}
		else
			if (serviceOn)
				menu.DrawServiceScreen(PinsUpdate(), encoderValue);
		
		kostyl = 0;
	}

	if (key != NO_KEY)
	{
		if (serviceOn) ServiceMode(key);
		else if (progRun) RunningMode(key);
		else if (programMode == 0) SelectMode(key);
		else if (isInputAmt) InputAmtMode(key);
		else InputPauseMode(key);
		menu.DrawMenu();  // Update Menu
	}
}

void UpdateSettings()
{
	if (eps != epsOld)
	{
		EEPROM.write(0, eps);
		epsOld = eps;
	}
	if (coolDown != coolDownOld)
	{
		EEPROM.write(1, highByte(coolDown));
		EEPROM.write(2, lowByte(coolDown));
		coolDownOld = coolDown;
	}
}

long inputs[3];
long* PinsUpdate()
{
	inputs[0] = digitalRead((long)pins::motorA);
	inputs[1] = digitalRead((long)pins::motorB);
	inputs[2] = digitalRead((long)pins::sound);
	return inputs;
}

void ServiceMode(char key)
{
	switch (key)
	{
	case '*':
		menu.DelLast();
		break;
	case '#':
		break;
	case 'B':
		menu.Up();
		break;
	case 'C':
		menu.Down();
		break;
	case 'D':
		serviceOn = false;
		menu.ApplySettings(lotMax, seriesMax, coolDown); // Get input
		//UpdateSettings();
		controlPins.SetCoolDown(coolDown); // Update eps and coolDown in controlPins
		menu.SetMenuMode(Menus::Select);
		menu.DrawMenu();
		break;
	default:
		menu.Input(key);
		break;
	}
}

void RunningMode(char key)
{
	switch (key)
	{
	case 'B':
		menu.Up();
		break;
	case 'C':
		menu.Down();
		break;
	case 'D':
		lcd.cursor();
		lcd.blink();
		progRun = false;
		//controlPins.Stop();
		//controlPins.Reset();
		programMode = 0;
		menu.SetMenuMode(Menus::Select);
		break;
	default:
		break;
	}
}

void InputPauseMode(char key)
{
	switch (key)
	{
	case '*':
		menu.DelLast();
		break;
	case '#':
		break;
	case 'B':
		break;
	case 'C':
		break;
	case 'D':
		isInputAmt = true;
		menu.SetMenuMode(Menus::InpAmt);
		break;
	case 'A':
		encoderValue = 0;
		menu.ApplyInpPause(pauseShift); //!!!!!
		lcd.noCursor();
		lcd.noBlink();
		//controlPins.Reset();
		//controlPins.Start(programs.leng, programs.amt, encoderCounter);
		progRun = true;
		menu.DrawRunScreen(lotMax, seriesMax); //!!!!!
		menu.SetMenuMode(Menus::Run);
		break;
	default:
		menu.Input(key);
		break;
	}
}

void InputAmtMode(char key)
{
	switch (key)
	{
	case '*':
		menu.DelLast();
		break;
	case '#':
		break;
	case 'B':
		menu.Up();
		break;
	case 'C':
		menu.Down();
		break;
	case 'D':
		isInputAmt = false;
		programMode = 0;
		menu.SetMenuMode(Menus::Select);
		break;
	case 'A':
		isInputAmt = false;
		encoderValue = 0;
		menu.ApplyInpAmt(lotMax, seriesMax);
		if (programMode == 2)
		{
			lcd.noCursor();
			lcd.noBlink();
			//controlPins.Reset();
			//controlPins.Start(programs.leng, programs.amt, encoderCounter);
			progRun = true;
			menu.DrawRunScreen(lotMax, seriesMax); //!!!!!
			menu.SetMenuMode(Menus::Run);
		}
		else // programMod == 3 
		{
			menu.SetMenuMode(Menus::InpPause);
		}
		break;
	default:
		menu.Input(key);
		break;
	}
}

void SelectMode(char key)
{
	cash = menu.getMenu();
	switch (key)
	{
	case '1':
		menu.DrawRunScreen(0, 0); //!!!!!
		menu.SetMenuMode(Menus::Run);
		progRun = true;
		programMode = 1;
		break;
	case '2':
		isInputAmt = true;
		menu.SetMenuMode(Menus::InpAmt);
		lcd.cursor();
		lcd.blink();
		programMode = 2;
		break;
	case '3':
		isInputAmt = true;
		menu.SetMenuMode(Menus::InpAmt);
		lcd.cursor();
		lcd.blink();
		programMode = 3;
		break;
	case '*': // Service
		serviceOn = true;
		menu.SetMenuMode(Menus::Service);
		menu.UpdateValues(lotMax, seriesMax, coolDown);
		break;
	default:
		menu.Input(key);
		break;
	}
}

void EncoderChange() // Interruption
{
	if (digitalRead(21) != 0) // Read encoderB
		encoderValue++;
	else
		encoderValue--;
};
