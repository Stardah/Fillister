#pragma once
#include <Arduino.h>

enum class pins {
	motorA = 20, motorB = 21, sensor = 21, sound = 24
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
	void Start(long newlength, long newparts, long encoderValue);
	void Stop();
	bool* ScanPins();
	void UpdateInputs(long encoderValue);
	void AnyKey() 
	{
		if (programMod == 2) 
			isPauseTime = false;
	};

	// Setters/getters
	//

	void SetCoolDown(long coolDown_)
	{
		coolDown = coolDown_;
	};

	void SetLotSeriaMod(long lot, long series, int mod) 
	{
		lotMax = lot;
		seriesMax = series;
		programMod = mod;
	};

	long GetLot(long encoderValue)
	{
		return encoderValue - initialLotValue;
	};
	long GetSeries(long encoderValue)
	{
		return encoderValue - initialSeriesValue;
	};
	~ControlPins();
private:
	void RunGear();
	void StopGear();
	void HandMode(long encoderValue);
	void HalfHandMod(long encoderValue);
	void AutoMod(long encoderValue);
	void Sound(long time);
	// All pins
	//
	bool engineA = false;	// out on - 0
	bool engineB = false;	// out on - 0
	bool sound = false;		// out on - 0
	bool runOn = false;
	// 
	bool firstIteration = true;
	bool changeFlag = false;
	bool isPauseTime = false;
	// Parameters
	//
	int programMod = 0;
	long lotMax;
	long seriesMax;
	long curLot = 0;
	long curSeries = 0;
	long initialLotValue; // Encoder value at the moment when lot started
	long initialSeriesValue; // Encoder value at the moment when seria started
	long coolDown = 0;
	long timer = 0;
};