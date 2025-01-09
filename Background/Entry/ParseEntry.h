#include "../../State/GUIState.h"
#include "../../State/EventHandler.h"
#include "../../GUI/Config/NotificationConfig.h"
#include "../Core/BaseAddressDumper.h"
#include  "../Core/Utils.h"

namespace ParseEntry
{
	void Entry()
	{
		if (MainMenuState.GetUEVersion == ProcessState::Start) {
			MainMenuState.GetUEVersion = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] GetUEVersion");
			DumperUtils.GetUEVersion();
			MainMenuState.GetUEVersion = ProcessState::Completed;
		}

		if (MainMenuState.GetFNamePool == ProcessState::Start) {
			MainMenuState.GetFNamePool = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get FName Pool");
			BaseAddressDumper.GetFNamePool();
			MainMenuState.GetFNamePool = ProcessState::Completed;
		}

		if (MainMenuState.GetGUObjectArray == ProcessState::Start) {
			MainMenuState.GetGUObjectArray = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get GUObject Array");
			BaseAddressDumper.GetGUObjectArray();
			MainMenuState.GetGUObjectArray = ProcessState::Completed;
		}

		if (MainMenuState.GetGWorld == ProcessState::Start) {
			MainMenuState.GetGWorld = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get GWorld");
			BaseAddressDumper.GetGWorld();
			MainMenuState.GetGWorld = ProcessState::Completed;
		}

		if (MainMenuState.ParseFNamePool == ProcessState::Start) {
			MainMenuState.ParseFNamePool = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Parse FName Pool");
			ParseMgr.ParseFNamePool();
			MainMenuState.ParseFNamePool = ProcessState::Completed;
		}

		if (MainMenuState.AutoConfig == ProcessState::Start) {
			MainMenuState.AutoConfig = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Auto Config");
			AutoConfig.AutoConfig();
			MainMenuState.AutoConfig = ProcessState::Completed;
		}

		if (MainMenuState.ParseGUObjectArray == ProcessState::Start) {
			MainMenuState.ParseGUObjectArray = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Parse GUObject Array");
			ParseMgr.ParseGUObjectArray();
			MainMenuState.ParseGUObjectArray = ProcessState::Completed;
		}

		if (MainMenuState.Prepareing == ProcessState::Start) {
			MainMenuState.Prepareing = ProcessState::Processing;

			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Prepareing");

			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get FName Pool");
			BaseAddressDumper.GetFNamePool();

			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get GUObject Array");
			BaseAddressDumper.GetGUObjectArray();

			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get Parse FName Pool");
			ParseMgr.ParseFNamePool();
			
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Auto Config");
			AutoConfig.AutoConfig();
			
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Parse GUObject Array");
			ParseMgr.ParseGUObjectArray();

			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, " ALL Completed !!! ");
			MainMenuState.Prepareing = ProcessState::Completed;
		}

		if (MainMenuState.ShowBaseAddress == ProcessState::Start or MainMenuState.ShowBaseAddress == ProcessState::Retry) {
			MainMenuState.ShowBaseAddress = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Show BaseAddress");
			if (BaseAddressDumper.ShowBaseAddress() == ProcessState::Retry) return;
			MainMenuState.ShowBaseAddress = ProcessState::Completed;
		}
	}
}