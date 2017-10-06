#include "ControlPins.h"

ControlPins::ControlPins(long error_)
{
	pinMode((long)pins::motorA, OUTPUT);
	pinMode((long)pins::motorB, OUTPUT);
	pinMode((long)pins::sound, OUTPUT);
	pinMode((long)pins::sensor, INPUT);
}

///
/// Clear variables, casts after stop()
///
void ControlPins::Reset()
{
	engineA = false;	// out
	engineB = false;	// out
	sound = false;		// out
	runOn = false;
	// 
	firstIteration = true;
}

///
/// Initialize variables
///
void ControlPins::Start(long newlotmax, long newseriamax, long encoderValue)
{
	lotMax = newlotmax;
	seriesMax = newseriamax;
	runOn = true;
	initialLotValue = encoderValue; // set init value for current lot
	initialSeriesValue = encoderValue;
	//if (ifAuto) Sound(700);
}


///
// Just stop the process, calls when has cut all parts
///
void ControlPins::Stop()
{
	runOn = false;
	StopGear();
	/*if (ifAuto)
	{
		Sound(300);
		delay(300);
		Sound(300);
		delay(300);
		Sound(300);
	}
	*/
}

///
// Runing gear whith current setups for vector and speed
///
void ControlPins::RunGear()
{
	//To DO: timer
	digitalWrite((long)pins::motorA, HIGH);
	digitalWrite((long)pins::motorB, LOW);
}

///
// Stops gear
///
void ControlPins::StopGear()
{
	digitalWrite((long)pins::motorA, HIGH);
	digitalWrite((long)pins::motorB, HIGH);
	digitalWrite((long)pins::sound, HIGH);
}

///
//Returns array of bool states of pins
///
bool* ControlPins::ScanPins()
{
	bool scan[5];
	for (long i = 0; i < 5; i++) scan[i] = true;
	return  scan;
}

///
// update states for all pins and do doings
///
// -1 OK
//  0 FULL_STOP
//  1 KNIFE
//  2
void ControlPins::UpdateInputs(long encoderValue)
{
	//initialLotValue = encoderCounter;

	//if (emergency) StopGear();
	if (programMod == 1)
		HandMode(encoderValue);	// Hand Mode
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
	firstIteration = true;
	RunGear();
	StopGear(); // if do not move then stop		
}

///
// Auto mode with pressany key on each seria completement
///
void ControlPins::HalfHandMod(long encoderValue)
{
	if (!isPauseTime)
	{
		RunGear();
		if ((encoderValue - initialSeriesValue) >= seriesMax) // It's time to cut but...
		{
			StopGear();			// Stop engine
			isPauseTime = true;	// Press anykey...
			initialSeriesValue = encoderValue; // Update seria start point
		}
		if ((encoderValue - initialLotValue) >= lotMax)
		{
			Stop();
		}
	}
}

///
// FullAuto mode with cooldown on each seria completement
///
void ControlPins::AutoMod(long encoderValue)
{
	if (isPauseTime)
	{
		++timer;
		if (timer > coolDown)
			isPauseTime = false;
	}
	else
	{
		RunGear();
		if ((encoderValue - initialSeriesValue) >= seriesMax) // It's time to cut but...
		{
			StopGear();			// Stop engine
			initialSeriesValue = encoderValue; // Update seria start point
			isPauseTime = true;	// Press anykey...
			timer = 0;			// Reset timer
		}
		if ((encoderValue - initialLotValue) >= lotMax)
		{
			Stop();
		}
	}
}

void ControlPins::Sound(long time)
{
	digitalWrite((long)pins::sound, LOW);
	delay(time);
	digitalWrite((long)pins::sound, HIGH);
}

ControlPins::~ControlPins()
{
}
