#pragma once
#include <Windows.h>

class timer {
public:
	timer() {
		initTime();
	}
	UINT64 getTime();
private:
	void initTime();
	UINT64 startTime;
};

void timer::initTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	printf("\n Timer : %lld", startTime);
}

inline UINT64 timer::getTime()
{
	UINT64 value;
	QueryPerformanceCounter((LARGE_INTEGER*)&value);
	return (value - startTime) * 0.001;
}