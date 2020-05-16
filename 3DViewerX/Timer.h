#pragma once

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Reset();
	void Tick();
	double DeltaTime();
	double TotalTime();

private:
	double m_SecondsPerCount = 0.0;
	double m_DeltaTime = 0.0;

	__int64 m_BaseTime = 0;
	__int64 m_PausedTime = 0;
	__int64 m_StopTime = 0;
	__int64 m_PrevTime = 0;
	__int64 m_CurrTime = 0;

	bool m_Stopped = false;
};