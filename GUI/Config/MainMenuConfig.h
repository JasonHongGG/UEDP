#pragma once
#include <map>
#include <vector>
#include "../../State/GlobalState.h"

enum class MainMenuCurPage
{
	Main = 0,
	Image = 1,
	Tool = 2,
	FX = 3,
	MenuConfigSL = 4,
	MainConfig = 5,
	DumperConsole = 6,
	Quit = 7,
};

enum class MainConfigTabType
{
	ColorStylePanel,
	ParamterPanel,
	ComponentPanel,
};

struct MainMenuConfig
{
	//Dumper Console
	size_t GUObjectArrayTotalObjectProgressBarTotalValue = 1000;
	ProcessState DumperGlobalExecution = ProcessState::Idle;		// 目前只有 Global 的執行會計時
	int DumperGlobalExecutionMode_SelectedIndex = 1;	// 預設是 Activate
	int DumperGlobalExecutionFilterMode_SelectedIndex = 0;
	std::vector<std::string> DumperGlobalExecutionMode = { "All", "Activate", "Deactivate" };
	std::vector<std::string> DumperGlobalExecutionFilterMode = { "All", "OpenName", "Category", "Activate", "Deactivate" };
	char DumperOptionFilterStr[128] = "";
	std::map<std::string, bool> IsDumperOptionActivate = {
		{"GetFNamePool", true},
		{"GetGUObjectArray", true},
		{"ParseFNamePool", true},
		{"ParseGUObjectArray", true}
	};


	// Config Page
	struct TabStruct {
		bool isOpen = false;
		std::string Name = "";
		MainConfigTabType Type = MainConfigTabType::ColorStylePanel;
	};
	std::vector<TabStruct> ConfigPageTabList = {
		{true, "ColorStyle", MainConfigTabType::ColorStylePanel},
		{true, "ParamterPanel", MainConfigTabType::ParamterPanel},
		{true, "ComponentPanel", MainConfigTabType::ComponentPanel},
	};
};

inline MainMenuConfig MainMenuConf;