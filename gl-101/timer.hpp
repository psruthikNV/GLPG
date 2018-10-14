#pragma once
#include <Windows.h>

class timer {
public:
	void initTime();
	UINT64 getTime();
	UINT64 getTimeElapsed();
private:
	UINT64 startTime;
};

void timer::initTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
}

inline UINT64 timer::getTime()
{
	UINT64 value;
	value = QueryPerformanceCounter((LARGE_INTEGER*)&value);
	return value;
}

inline UINT64 timer::getTimeElapsed()
{
	UINT64 value;
	value = QueryPerformanceCounter((LARGE_INTEGER*)&value);
	return  value - startTime;
}