#pragma once
#include <Arduino.h>

enum class pins {
	encoderA = 20, encoderB = 21, knife = 50, forRev1 = 48, forRev2 = 46,
	handDrive1 = 44, handDrive2 = 42, emergency = 40, handAuto = 38,
	gearRev = 22, gearForv = 24, gearSpeed = 23, sound = 25
};

class ControlPins
{
public:
	ControlPins(long error);
	static bool ReadPin(long num)
	{
		return bool(digitalRead(num));
	};
	void Reset();
	void Start(long newlength, long newparts, long encoderCounter);
	void Stop();
	bool* ScanPins();
	long UpdateInputs(long encoderCounter);

	void SetEpsCool(long eps_, long coolDown_, long nozh_)
	{
		eps = eps_;
		coolDown = coolDown_;
		nozh = nozh_;
	};
	long GetLength()
	{
		//if (knifeSwitch) return length;
		return encoderCounterRef - encoderLength;
	};
	long GetParts()
	{
		return encoderParts;
	};

	bool GetAuto()
	{
		return ifAuto;
	};
	~ControlPins();
private:
	void RunGear();
	void StopGear();
	void HandMode(long encoderCounter);
	long AutoMod(long encoderCounter);
	void Sound(long time);
	// Hand 
	bool forRev1 = false;
	bool forRev2 = false;
	bool handDrive1 = false;
	bool handDrive2 = false;
	// All
	bool emergency = false;
	// Auto
	bool ifAuto = false;
	bool gearForv = true;	// out
	bool gearSpeed = true;	// out
							//bool sound = false;		// out
	bool knife = false;

	bool knifeSwitch = false;
	bool runOn = false;
	bool rollback = false;
	bool firstIteration = true;
	//long encoderCounter = 0; // mm counter
	long encoderLength = 0;
	long encoderParts = 0;
	long encoderCounterRef;
	long length = 0;
	long parts = 0;
	long eps = 0;
	long coolDown = 0;
	long nozh = 5;
	long kostyl = 0;
	long notify = -1;
};