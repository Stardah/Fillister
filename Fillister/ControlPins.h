#pragma once
#include <Arduino.h>

enum class pins {
	motorA = 22, motorB = 23, sensor = 21, sensorInt = 2, sound = 26, power = 19, powerInt = 4, lastone = 25
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
	void Start(long newLotMax, long newSeriaMax, int mod, long encoderValue);
	void Stop();
	void UpdateInputs(long encoderValue);
	void AnyKey();

	// Setters/getters
	//

	void SetCoolDown(long coolDown_)
	{
		coolDown = coolDown_*1000;
	};

	void SetInitialSeries(long series)
	{
		initialSeriesValue = series;
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
	void StopGear();
private:
	void RunGear();
	void DisableGear();
	void HandMode(long encoderValue);
	void HalfHandMod(long encoderValue);
	void AutoMod(long encoderValue);
	void Sound(long time);
	// All pins
	//
	bool engineA = false;	// out on - 0
	bool engineB = false;	// out on - 0
	bool sound = false;		// out on - 0
	// 
	bool isPauseTime = false;
	// Parameters
	//
	int programMod = 0;
	long lotMax;
	long seriesMax;
	long initialLotValue; // Encoder value at the moment when lot started
	long initialSeriesValue; // Encoder value at the moment when seria started
	long coolDown = 0;
	long pauseTimer = 0;
};