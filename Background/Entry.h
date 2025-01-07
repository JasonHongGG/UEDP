#pragma once
#pragma once
#include "../System/Process.h"

class BackgroundEntry {
public:
	void InitialProcess();

	void CloseProcess();

	void Entry();

	void SetUp()
	{
		 //Busy Box (use sleep to realse cpu)
		while (Process::ProcState == Process::ProcessState::Running) {
			Entry();
			Sleep(10);
		}
	}
private:
};



inline BackgroundEntry Background = BackgroundEntry();