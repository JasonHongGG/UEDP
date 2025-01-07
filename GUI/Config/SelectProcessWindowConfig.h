#pragma once
#include <string>
#include <vector>

struct SelectProcessWindowConfig
{
	int WindowTitleSelectIndex = 0;
	int WindowTitleDisplaySelectIndex = 0;		// Selected Window 用的，確定結果會給 WindowTitle_SelectIndex
	std::string WindowTitleFilterStr = "";
	std::vector<std::string> WindowTitleList = { "None" };
};
inline SelectProcessWindowConfig SelectProcessWindowConf;