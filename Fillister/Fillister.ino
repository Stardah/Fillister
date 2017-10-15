/*
 Name:		Fillister.ino
 Created:	06.10.2017 15:36:06
 Author:	Adrax
*/

#include <Key.h>
#include <Keypad.h>
#include "Menu.h" 
#include "ControlPins.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Display
LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

// Keypad-------------------------------------------------------------------
//

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] =
{
	{ '1','2','3','A' },
	{ '4','5','6','B' },
	{ '7','8','9','C' },
	{ '*','0','#','D' }
};
byte rowPins[ROWS] = { 66, 67, 68, 69 }; // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 62, 63, 64, 65 }; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//-------------------------------------------------------------------------

// External classes
//

Menu menu(lcd);	// Menu update
ControlPins controlPins(20); // Pins state update, error = 20

Menus cash = Menus::Select; // Save previous menu
bool progRun = false; // Access to write program
bool serviceOn = false; // Service mode on
bool isInputAmt = false;
volatile long encoderValue = 0; // Encoder mm counter

// Settings
//

long timer = 0;
long programMod = 0;
long lotMax = 10;
long seriesMax = 2;
int pauseShift = 4;
long probeg = 0;

unsigned long starttime;

void setup()
{
	// Read settings
	ReadData();
	controlPins.SetCoolDown(pauseShift);
	// Setup
	lcd.begin(16, 2);
	menu.DrawMenu();

	attachInterrupt(2, EncoderChange, FALLING);
	attachInterrupt(1, PowerDown, RISING);
}

void loop()
{
	// Update Input
	//
	char key = keypad.getKey();
	
	// Update Gear
	//
	timer++;
	if (timer > 3000)
	{
		// Update Vcc
		//
		if (progRun)
		{
			// Mode 1
			if (programMod == 1)
			{
				menu.DrawCounterScreen(encoderValue); // Update Counter Screen
				menu.DrawMenu();
			}
			
			// Mode 2 or 3
			else
			{
				controlPins.UpdateInputs(encoderValue);
			
				// Perfomance
				if (micros() - starttime>10000000)
				{
					/*Serial.print(micros());
					Serial.print(" - ");
					Serial.print(starttime);
					Serial.print(" = ");
					Serial.println(micros() - starttime);
					*/
					starttime = micros();
					menu.SetPerfomance(controlPins.GetLot(encoderValue) * 6);
				}
				// Update Run Screen
				menu.DrawRunScreen(controlPins.GetLot(encoderValue), controlPins.GetSeries(encoderValue)); // Display current values
				
				// Redraw Screen
				if (controlPins.GetLot(encoderValue) >= lotMax)
				{
					probeg += controlPins.GetLot(encoderValue);
					menu.DrawMenu();
					delay(4000);
					progRun = false;
					programMod = 0;
					controlPins.Reset();
					menu.SetMenuMode(Menus::Select);
					menu.DrawMenu();
				}
				else 
					menu.DrawMenu();
			}				
		}
		else
			if (serviceOn)
				menu.DrawServiceScreen(PinsUpdate(), encoderValue, probeg);
		timer = 0;
	}

	if (key != NO_KEY)
	{
		if (serviceOn) ServiceMode(key);
		else if (progRun) RunningMode(key);
		else if (programMod == 0) SelectMode(key);
		else if (isInputAmt) InputAmtMode(key);
		else InputPauseMode(key);
		menu.DrawMenu();  // Update Menu
	}
}


//    addr			var			type
// 	   0		programMode		int
//  1,2,3,4		curLot			long
//  5,6,7,8		lotMax			long
// 9,10,11,12	seriesMax		long
//	13,14		pauseShift		int
// 15,16,17,18	probeg			long


void SaveData() 
{
	if (EEPROM.read(0)!=programMod)
		EEPROM.write(0, programMod);
	
	switch (programMod)
	{
	case 2:
		WriteLong(1, encoderValue);
		WriteLong(5, lotMax);
		WriteLong(9, seriesMax);
		break;
	case 3:
		WriteLong(1, encoderValue);
		WriteLong(5, lotMax);
		WriteLong(9, seriesMax);
		EEPROM.write(13, highByte(pauseShift));
		EEPROM.write(14, lowByte(pauseShift));
		break;
	default:
		break;
	}
	WriteLong(15, probeg);
}

void ReadData() 
{
	// Read previous program mod
	//
	programMod = EEPROM.read(0);
	
	// Recover data
	//
	switch (programMod)
	{
	case 1:
		RunMod1();
		break;
	case 2:
		encoderValue = ReadLong(1);
		lotMax = ReadLong(5);
		seriesMax = ReadLong(9);
		RunMod2(lotMax, seriesMax);
		break;
	case 3:
		encoderValue = ReadLong(1);
		lotMax = ReadLong(5);
		seriesMax = ReadLong(9);
		pauseShift = EEPROM.read(13) * 256 + EEPROM.read(14);
		RunMod3(lotMax, seriesMax, pauseShift);
		Serial.print(pauseShift);
		Serial.println();
		break;
	default:
		programMod = 0;
		break;
	}

	probeg = ReadLong(15);
}

void RunMod1()
{
	menu.DrawCounterScreen(0);
	menu.SetMenuMode(Menus::Count);
	progRun = true;
	encoderValue = 0;
	programMod = 1;
}

void RunMod2(long lotMax, long seriesMax)
{
	progRun = true;
	starttime = micros();
	controlPins.Reset();
	controlPins.Start(lotMax, seriesMax, 2, 0);
	menu.SetParameters(lotMax, seriesMax, 0);
	menu.DrawRunScreen(encoderValue, 0);
	menu.SetMenuMode(Menus::Run);
}

void RunMod3(long lotMax, long seriesMax, int pauseShift)
{
	progRun = true;
	starttime = micros();
	controlPins.Reset();
	controlPins.SetCoolDown(pauseShift);
	controlPins.Start(lotMax, seriesMax, 3, 0);
	controlPins.SetInitialSeries(encoderValue);
	menu.SetParameters(lotMax, seriesMax, pauseShift);
	menu.DrawRunScreen(encoderValue, 0);
	menu.SetMenuMode(Menus::Run);

	Serial.print("Mod3");
	Serial.println();
}

void WriteLong(int address, long value)
{
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}

long ReadLong(long address)
{
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
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
	case 'B':
		menu.Up();
		break;
	case 'C':
		menu.Down();
		break;
	case '*':
		serviceOn = false;
		lcd.noBlink();
		lcd.noCursor();
		menu.SetMenuMode(Menus::Select);
		menu.DrawMenu();
		break;
	default:
		//menu.Input(key);
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
	case 'A':
		progRun = false;
		if (programMod != 1)
		{
			controlPins.Stop();
			controlPins.Reset();
		}
		programMod = 0;
		menu.SetMenuMode(Menus::Select);
		break;
	default:
		controlPins.AnyKey();
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
	case 'A':
		isInputAmt = true;
		menu.SetMenuMode(Menus::InpAmt);
		break;
	case 'D':
		progRun = true;
		lcd.noCursor();
		lcd.noBlink();
		encoderValue = 0;
		starttime = micros();
		menu.ApplyInpPause(pauseShift);
		controlPins.Reset();
		controlPins.SetCoolDown(pauseShift);
		controlPins.Start(lotMax, seriesMax, programMod, encoderValue);
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
	case 'A':
		isInputAmt = false;
		lcd.noCursor();
		lcd.noBlink();
		programMod = 0;
		menu.SetMenuMode(Menus::Select);
		break;
	case 'D':
		isInputAmt = false;
		starttime = micros();
		encoderValue = 0;
		menu.ApplyInpAmt(lotMax, seriesMax);
		if (programMod == 2)
		{
			lcd.noCursor();
			lcd.noBlink();
			controlPins.Reset();
			controlPins.Start(lotMax, seriesMax, programMod, encoderValue);
			progRun = true;
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
		RunMod1();
		break;
	case '2':
		isInputAmt = true;
		lcd.cursor();
		lcd.blink();
		menu.SetMenuMode(Menus::InpAmt);
		programMod = 2;
		break;
	case '3':
		isInputAmt = true;
		lcd.cursor();
		lcd.blink();

		menu.SetMenuMode(Menus::InpAmt);
		programMod = 3;
		break;
	case '*': // Service
		serviceOn = true;
		lcd.cursor();
		lcd.blink();

		menu.SetMenuMode(Menus::Service);
		break;
	default:
		menu.Input(key);
		break;
	}
}

void EncoderChange() // Interruption
{
	if (digitalRead((int)pins::sensor) != 0) // Read encoderB
		encoderValue--;
	else
		encoderValue++;
}

void PowerDown()
{
	if (digitalRead((int)pins::power) == 1) // Read encoderB
		SaveData();
}