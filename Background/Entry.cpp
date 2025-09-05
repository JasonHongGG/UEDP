#pragma once
#include "Entry.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include "d3d11.h"
#include "../imgui/imgui.h"


#include "../State/GUIState.h"
#include "../State/EventHandler.h"

#include "Entry/ParseEntry.h"
#include "Entry/ConsoleEntry.h"
#include "Entry/PackageViewEntry.h"
#include "Entry/InspectorEntry.h"
#include "Entry/APIEntry.h"
#include "Entry/LoadAndSaveSettingEntry.h"
#include "Entry/ObjectGraphEntry.h"
#include "Entry/ProcessEntry.h"
#include "../Utils/Utils.h"
#include "../Utils/Env.h"



void BackgroundEntry::Entry()
{
	ProcessEntry::Entry();

	// Already Attach Process
	if (ProcessWindow::MainWindow) 
	{
		ParseEntry::Entry();
		ConsoleEntry::Entry();
		PackageViewEntry::Entry();
		InspectorEntry::Entry();
		APIEntry::Entry();
		ObjectGraphEntry::Entry();
	}
	APIEntry::FileEntry();
	LoadAndSaveSettingEntry::Entry();
}

void BackgroundEntry::Trigger()
{
	//Busy Box (use sleep to realse cpu)
	while (Process::ProcState == Process::State::Running) {
		Entry();
		Sleep(10);
	}
}