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
	ProcessState DumperGlobalExecution = ProcessState::Idle;		// �ثe�u�� Global ������|�p��
	int DumperGlobalExecutionMode_SelectedIndex = 1;	// �w�]�O Activate
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

	size_t ParseFNamePool_ProgressBar_Value = 0;
	size_t ParseFNamePool_ProgressBar_TotalValue = 0;
	ProcessState ParseFNamePool_ProgressBar = ProcessState::Idle;

	size_t ParseGUObjectArray_ProgressBar_Value = 0;
	size_t ParseGUObjectArray_ProgressBar_TotalValue = 0;
	size_t ParseGUObjectArray_ProgressBar_Index = 0;

	size_t TotalObject_Counter_ProgressBar_Value = 0;
	size_t TotalObject_Counter_ProgressBar_TotalValue = 1000;
	
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