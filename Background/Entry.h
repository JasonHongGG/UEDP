#pragma once
#pragma once
#include "../System/Process.h"
//#include "../gui/MenuConfig.h"
//#include "../gui/Overlay.h"

class BackgroundEntry {
public:
	void InitialProcess();

	void CloseProcess();

	void MainEntry();

	void SetUp()
	{
		// Busy Box (use sleep to realse cpu)
		//while (OverlayMgr.MenuState) {
		//	MainEntry();
		//	Sleep(10);
		//}
	}
private:
};



inline BackgroundEntry Background = BackgroundEntry();