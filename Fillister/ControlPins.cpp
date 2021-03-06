#include "ControlPins.h"

ControlPins::ControlPins(long error_)
{
	pinMode((long)pins::motorA, OUTPUT);
	pinMode((long)pins::motorB, OUTPUT);
	pinMode((long)pins::sound, OUTPUT);
	pinMode((long)pins::lastone, OUTPUT);
	pinMode((long)pins::sensor, INPUT);
	pinMode((long)pins::power, INPUT);
	digitalWrite((long)pins::sound, LOW);
	digitalWrite((long)pins::motorA, HIGH);
	digitalWrite((long)pins::motorB, HIGH);
	digitalWrite((long)pins::power, HIGH);
	digitalWrite((long)pins::lastone, HIGH);
}

///
// Clear variables, casts after stop()
///
void ControlPins::Reset()
{
	engineA = false;	// out
	engineB = false;	// out
	sound = false;		// out
}

///
// Initialize variables
///
void ControlPins::Start(long newlotmax, long newseriamax, int mod, long currentLot)
{
	isPauseTime = false;
	programMod = mod;
	lotMax = newlotmax;
	seriesMax = newseriamax;
	initialLotValue = currentLot; // set init value for current lot
	//initialSeriesValue = encoderValue;
}


///
// Just stop the process, calls when has cut all parts
///
void ControlPins::Stop()
{
	if (programMod == 3)
	{
		//Sound(100);
	}
}

///
// Runing gear whith current setups for vector and speed
///
void ControlPins::RunGear()
{
	digitalWrite((long)pins::motorA, HIGH);
	digitalWrite((long)pins::motorB, LOW);
	digitalWrite((long)pins::lastone, LOW);
	delay(450);
	DisableGear();
}

///
// Stops gear
///
void ControlPins::StopGear()
{
	digitalWrite((long)pins::motorA, LOW);
	digitalWrite((long)pins::motorB, HIGH);
	digitalWrite((long)pins::motorB, HIGH);
	delay(450);
	DisableGear();
}

///
// Disable motor
///
void ControlPins::DisableGear()
{
	digitalWrite((long)pins::motorA, HIGH);
	digitalWrite((long)pins::motorB, HIGH);
}

///
// Update states for all pins and do doings
///
void ControlPins::UpdateInputs(long encoderValue)
{
	if (programMod == 1)
		HandMode(encoderValue);
	else if (programMod == 2)
		HalfHandMod(encoderValue);
	else if (programMod == 3)
		AutoMod(encoderValue);
}

///
// When emploee controls the process himself
///
void ControlPins::HandMode(long encoderValue)
{
	initialSeriesValue = encoderValue;
}

///
// AnyKey press
///
void ControlPins::AnyKey()
{
	if (programMod == 2 && isPauseTime)
	{
		isPauseTime = false;
		RunGear();
	}
}

///
// Auto mode with pressany key on each seria completement
///
void ControlPins::HalfHandMod(long encoderValue)
{
	if (!isPauseTime)
	{
		if ((encoderValue - initialSeriesValue) >= seriesMax)
		{
			StopGear();			// Stop engine
			isPauseTime = true;	// Press anykey...
			initialSeriesValue = encoderValue; // Update seria start point
			Sound(100);
		}
		if ((encoderValue - initialLotValue) >= lotMax)
		{
			StopGear();
			//Stop();
			Sound(200);
		}
	}
	else 
	{
		Sound(100);
	}
}

///
// FullAuto mode with cooldown on each seria completement
///
void ControlPins::AutoMod(long encoderValue)
{
	if (isPauseTime)
	{
		delay(coolDown);
		isPauseTime = false;
		RunGear();
	}
	else
	{
		if ((encoderValue - initialSeriesValue) >= seriesMax) // It's time to cut but...
		{
			StopGear();			// Stop engine
			initialSeriesValue = encoderValue; // Update seria start point
			isPauseTime = true;	// Press anykey...
		}
		if ((encoderValue - initialLotValue) >= lotMax)
		{
			Stop();
		}
	}
}

void ControlPins::Sound(long mls)
{
	digitalWrite((long)pins::sound, HIGH);
	delay(mls);
	digitalWrite((long)pins::sound, LOW);
}

ControlPins::~ControlPins()
{
}
