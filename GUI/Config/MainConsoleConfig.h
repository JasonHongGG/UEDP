#pragma once
#include <vector>
#include "../Interface/DumperConsoleInterface.h"
#include "../../Font/IconsFontAwesome6.h"

enum class DumperConsoleCurPage
{
    Console = 0,
    PackageViewer = 1,
    Inspector = 2,
    API = 3
};

struct MainConsoleConfig
{
    std::vector<DumperItem> DumperTable = {};
    FNameInfoStruct FNameInfo;
    GUObjectInfoStruct GUObjectInfo;

    
	bool AddButtonIsHovered = false;
    std::string AddButtonStr = ICON_FA_CIRCLE_ARROW_DOWN;
    float AddButtonInitialWidth = 30.0f;
    float AddButtonWidth = AddButtonInitialWidth;

    bool ParseButtonIsHovered = false;
    std::string ParseButtonStr = ICON_FA_CIRCLE_ARROW_DOWN;
    float ParseButtonInitialWidth = 30.0f;
    float ParseButtonWidth = ParseButtonInitialWidth;
};
inline MainConsoleConfig MainConsoleConf;