#pragma once
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../GUI/Config/LoadAndSaveSettingConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../../Json/JsonManager.h"

namespace LoadAndSaveSettingEntry
{
	inline void Entry()
	{
		if (LoadAndSaveSettingState.SaveEvent.State == ProcessState::Start) {
			LoadAndSaveSettingState.SaveEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Save Menu Setting File");
			JsonMgr.WriteMenuSettingFile();
			LoadAndSaveSettingState.SaveEvent.State = ProcessState::Completed;
		}

		if (LoadAndSaveSettingState.ReadEvent.State == ProcessState::Start) {
			LoadAndSaveSettingState.ReadEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Load Menu Setting File");
			JsonMgr.ReadMenuSettingFile();
			LoadAndSaveSettingState.ReadEvent.State = ProcessState::Completed;
		}

		if (LoadAndSaveSettingState.DeleteEvent.State == ProcessState::Start) {
			LoadAndSaveSettingState.DeleteEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Delete Menu Setting");
			JsonMgr.DeleteMenuSettingFile(LoadAndSaveSettingState.CurTime);
			LoadAndSaveSettingState.DeleteEvent.State = ProcessState::Completed;
		}

		if (LoadAndSaveSettingState.LoadEvent.State == ProcessState::Start) {
			LoadAndSaveSettingState.LoadEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Load Menu Setting");
			JsonMgr.LoadMenuSettingFile(LoadAndSaveSettingState.CurTime);
			LoadAndSaveSettingState.LoadEvent.State = ProcessState::Completed;
		}
	}
}