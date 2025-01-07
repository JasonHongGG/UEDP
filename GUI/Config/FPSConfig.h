#pragma once
#include <Windows.h>

struct FPSConfig
{
	bool Initial = true;
	float TargetFPS = 60;
	double TargetFrameRate = 1.0 / TargetFPS;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER CurrentTime;
	LARGE_INTEGER PreviousTime;
};

inline FPSConfig FPSConf;