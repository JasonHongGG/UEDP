#pragma once

enum class ProcessState {
	Idle = 0,
	Start = 1,
	Processing = 2,
	Completed = 3,
	Retry = 4
};

enum class ValueType {
	Byte = 0,
	Int16 = 1,
	Int32,
	Int64,
	Float,
	Double,
	String,
};

namespace GlobalState 
{
}