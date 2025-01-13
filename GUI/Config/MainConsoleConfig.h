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


struct PackageViwerConfig
{
    bool PackageDataResizeable = true;
    float PackageDataWindowWidth = 200;
    float PackageObjectWindowWidth = 300;
    float GlobalSearchPackageObjectWindowWidth = 20;
    float ObjectTypeIndentation = 50;
    float ObjectNameindentation = 450;


    // ScrollY Position
    std::string SetPackageDataListScrollYPos = "";
    std::string SetObjectDataListScrollYPos = "";


    int PackageDataListSelectIndex = -1;
    std::string PackageDataListFilterStr = "";
    std::vector<std::string> PackageDataList = { "1", "2", "3", "4", "5", "6" };
    

    enum SortMode {
        Object = 0,
        Funct = 1
    };
    int PackageObjectDataSortMode = SortMode::Object;
    std::map<std::string, int> PackageObjectDataTabFlagMap = { {"Class", ImGuiTabItemFlags_None}, {"Struct", ImGuiTabItemFlags_None}, {"Enum", ImGuiTabItemFlags_None}, {"Function", ImGuiTabItemFlags_None} };
    int PackageObjectDataListSelectIndex = -1;
    std::string PackageObjectDataFilterStr = "";
    std::map<std::string, std::map<std::string, std::pair<std::string, DWORD_PTR>>> PackageObjectDataList = {};
    

    enum GlobalSearchMode {
        ObjectName = 0,
        MemberName = 1,
    };
    struct GlobalSearchObject {
        std::string PackageName = "";
        std::string TabName = "";
        std::string ObjectName = "";
        std::string MemberName = "";
        DWORD_PTR Address = NULL;
    };
    int GlobalSearchMode = GlobalSearchMode::ObjectName;
    int GlobalSearchListSelectIndex = -1;
    std::string GlobalSearchStr = "";
    std::map<std::string, GlobalSearchObject> GlobalSearchList;	// ObjectName : ObjectData
    

    int ObjectContentSerchIdx = 1;
    std::string ObjectContentSearchStr = "";
    std::vector<std::string> ObjectContentTabNameList = { "Tab1", "Tab2" };
    std::map<DWORD_PTR, bool> ObjectContentTabOpenState = { {0x123456,true}, {0x456789,true} };
    std::map<std::string, DumperObject> ObjectContentMap = {};
    bool GetPackageObjectByName(std::string Name, DumperObject& Object) {
        auto iter = ObjectContentMap.find(Name);
        if (iter != ObjectContentMap.end()) {
            Object = iter->second;
            return true;
        }
        else return false;
    }
};
inline PackageViwerConfig PackageViwerConf;