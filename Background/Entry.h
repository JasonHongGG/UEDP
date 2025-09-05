#pragma once
#pragma once
#include "../System/Process.h"

class BackgroundEntry {
public:
	void Entry();
	void Trigger();
private:
};

inline BackgroundEntry Background = BackgroundEntry();