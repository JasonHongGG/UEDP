#pragma once
#include <vector>
#include <stack>
#include "../Interface/DumperConsoleInterface.h"
#include "../../Font/IconsFontAwesome6.h"
#include "../../State/GlobalState.h"

enum class DumperConsoleCurPage
{
    Console = 0,
    PackageViewer = 1,
    Inspector = 2,
    API = 3
};

enum class ProcessClass {
    C_DumperConsole = 0,
    C_PackageViewer = 1,
    C_Inspector = 2,
};

struct DisplayNextStateStruct {
    DWORD_PTR NextAddress = 0x0;
    size_t NextOffest = 0x0;
    size_t NextIndentation = 0x0;
    bool ShowHeader = true;
    size_t Size = 1;    //最少就是顯示一次 (在 EditorEnable 關閉時，用來展示該物件是甚麼)
};
inline DisplayNextStateStruct DisplayNextState;

struct MainConsoleConfig
{
    ImVec2 CurrentWindowPosition = { 0, 0 };
    ImVec2 CurentWindowSize = { 0 ,0 };
    std::vector<DumperItem> DumperTable = {};
    FNameInfoStruct FNameInfo;
    GUObjectInfoStruct GUObjectInfo;
    ImGuiTreeNodeFlags TreeNodeExFlag = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
    ImGuiTreeNodeFlags TreeNodeExLeafFlag = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

    
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
    int PackageDataResizeable = true;
    int PackageObjectResizeable = true;
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
    std::string PackageObjectDataListFilterStr = "";
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


struct InspectorConfig
{
    // Style Setting Parameter
    int IndentationIndependent = false;
    int IndentationAccumulate = true;
    int AlignGroupObjectIndentation = true;
    int InspectorListResizeable = true;
    float InspectorList_Width = 200;
    int DerivedListResizeable = true;
    float DerivedList_Width = 200;
    float InspectorOffsetIndentation = 20;
    float InspectorTypeIndentation = 70;
    float InspectorNameIndentation = 300;
    float InspectorObjectAddressIndentation = 600;
    float InspectorAddressIndentation = 750;
    float InspectorValueIndentation = 900;
    float InspectorBaseIndentation = 50;

    float AdditionIndentWidth = 10;		// tree 展開的 node 前有一個剪頭符號也會占空間

    int InspectorTreeNavLine = true;
    float InspectorTreeNavLineOffset = 10.f;		// 所有 TreeNavLine 的偏移 => 主軸以及Branch長度
    float InspectorTreeObjectNavLineOffset = 35.f;
    float InspectorTreeMemberNavLineOffset = 25.f;
    float InspectorTreeNavBranchLineSize = 0.f;



	// Inspector List
    enum InspectorListMode {
        Derived = 0,
        Super = 1
    };
    struct  InspectorListObject {
        std::string Name = "";
        std::string Type = "";
        DWORD_PTR Address = NULL;
        int Mode = InspectorListMode::Super;
        int EditorEnable = false;
        int IsInstance = false;
    };
    struct DerivedListStruct {
        bool IsOpen = false;
        std::string CurOpenName = "";
        std::string FilterStr = "";
        int SelectIndex = -1;
        std::map<std::string, std::vector<InspectorListObject>> Map;
    };
    DerivedListStruct DerivedList;
    std::string InputStr;
    int InspectorListMode = InspectorListMode::Super;
    int InspectorListSelectIndex = -1;
    std::string InspectorListFilterStr = "";
    std::vector<InspectorListObject>InspectorList = { };
    



    
    // Object Content Tree Nav Line
    enum TreeNavLine {
        Start = 0,		// Tree 開始時
        Update = 1,		// 每個顯示一個 Item 都會更新 End 
        Draw = 2,		// Tree 結束時
    };
    float RelativeDistanctWithOffset = 0.f;
    std::stack<ImVec2> NavLineStartStack;
    std::stack<ImVec2> NavLineEndStack;
    int InspectorObjectContentSearchIdx = 1;
    std::string InspectorObjectContentSearchStr = "";
    std::map<std::string, std::vector<DumperObject> > InspectorObjectContentMap;



    // Object Instance Sarch
    enum SearchMode {
        Normal = 0,
        Condition = 1,
    };
    struct ConditionObject
    {
        bool IsOpen = false;
        int ID = -1;
        int TypeListSelectIndex = 2;// Default Int32;
        ValueType Type = ValueType::Int32;
        std::string DisplayType = "Int";
        std::string Offset = "";
        std::string Value = "";
        std::vector<std::string> Pointer;
        bool PointerEnable = false;
    };
    // Serch Panel
    std::string ObjectInstanceSearchInputStr = "";
    int ObjectInstanceSearchMapSelectedIndex = -1;
    std::map<std::string, DWORD_PTR> ObjectInstanceSearchMap;	// str(Address) : Address
    // Option Panel
    int ObjectInstanceSearchMode = SearchMode::Normal;
    std::vector<std::string> TypeList = { "Byte", "Int16", "Int", "Int64", "Float", "Double", "String" };	//需要和 ValueType 同步更新 
    int TypeListSelectIndex = 2;		// Default Int32
    std::vector<ConditionObject> ConditionSet = { ConditionObject() };



};
inline InspectorConfig InspectorConf;

struct APIConfig
{
    int SearchIdx = 1;
    std::string SearchStr = "";
    std::vector<DumperItem> APIItemList = {};
};
inline APIConfig APIConf;

class EditorMemoryManager
{
public:
    struct Storage {
        std::map<std::string, int32_t> IntType;
        std::map<std::string, float> FloatType;
        std::map<std::string, bool> BoolType;
        std::map<std::string, std::pair<std::string, int32_t>> EnumType;
        std::map<int32_t, std::string> NameType;
    };
    std::map<DWORD_PTR, Storage> ObjectStroageData;	//每個 Object Instance 都有自己的 Storage，但 Object 參考是相同的

    template<typename T, typename U = std::string>
    bool MapInitIfNotExist(std::map<U, T>& myMap, const U& key, const T& value) {
        if (myMap.find(key) == myMap.end()) {
            myMap[key] = value;
            return false;
        }
        return true;
    }

private:
};
inline EditorMemoryManager EditorMemMgr = EditorMemoryManager();