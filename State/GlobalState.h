#pragma once

enum class ProcessState {
	Idle = 0,
	Start = 1,
	Processing = 2,
	Completed = 3,
	Retry = 4
};

namespace GlobalState 
{
}