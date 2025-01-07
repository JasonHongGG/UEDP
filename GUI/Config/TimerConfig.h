#pragma once
#include <chrono>
#include <map>

struct TimerConfig
{
	struct TimerObj {
		bool TimerActivate = false;
		std::chrono::steady_clock::time_point StartTime;
		std::chrono::steady_clock::time_point EndTime;
		std::chrono::steady_clock::time_point CurTime;
	};
	std::map<std::string, TimerObj> TimeObjMap = { 
		{"DumperTimer", TimerObj()},
		{"InspectorMemoryScanTimer", TimerObj()}
	};

};
inline TimerConfig TimerConf;