#pragma once
#include <string>
#include <Windows.h>
#include <shared_mutex>
#include "../../../imgui/imgui.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"
#include "../../Config/StyleConfig.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../MyGuiComponent.h"
#include "../../../State/EventHandler.h"
#include "../../Interface/DumperConsoleInterface.h"

namespace PackageViewer
{
    inline bool PackageDataResizeable = true;
    inline ImGuiChildFlags ChildWindowFlag = ImGuiChildFlags_Border;
    inline std::shared_mutex GlobalSearchObjectLock;
    int SearchMatchCnt = 0;
    bool SearchFocus = false;

    void ClickableObjectBtn(std::string DispalyName, DWORD_PTR Address);

    void PackageListRender();
    void ObjectInfoListRender();
    void GlobalSearchListRender();
    void ObjectContentRender();

	void Render(DumperConsoleCurPage& PageID) {
        if (PageID != DumperConsoleCurPage::PackageViewer) return;
        PackageListRender();
        ObjectInfoListRender();
        GlobalSearchListRender();
        ObjectContentRender();
	}
}

void PackageViewer::ClickableObjectBtn(std::string DispalyName, DWORD_PTR Address)
{
    const float FrameHeight = ImGui::CalcTextSize("Inheritance", NULL, true).y;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    if (ImGui::Button(std::string(DispalyName + "##ClickableObjectBtn").c_str(), ImVec2(0, FrameHeight)))
    {
        EventHandler::OpenPackageObjectTab(DispalyName, Address);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);   //將滑鼠設置成 "手"
    ImGui::PopStyleVar();

}

// ============================================================================================
//                                  Page Render
// ============================================================================================

void PackageViewer::PackageListRender()
{
    if (PackageViwerConf.PackageDataResizeable) ChildWindowFlag |= ImGuiChildFlags_ResizeX;
    else                                        ChildWindowFlag &= ~ImGuiChildFlags_ResizeX;
    ImGui::BeginChild("PackageData", ImVec2(PackageViwerConf.PackageDataWindowWidth, 0), ChildWindowFlag);
    {
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("%d Package", PackageViwerConf.PackageDataList.size());
        ImGui::PopFont();
        MyListBox("##Package List", PackageViwerConf.PackageDataListSelectIndex, PackageViwerConf.PackageDataList, ImVec2(-FLT_MIN, -FLT_MIN - 30), PackageViwerConf.PackageDataListFilterStr)
            .SetSelectCallBack([&](int i) {
            EventHandler::UpdataPackageObjectList(PackageViwerConf.PackageDataList[i]);
                })
            .SetPostCallBack([&](int i) {
                    // 頁面位置
                    if (PackageViwerConf.PackageDataList[i] == PackageViwerConf.SetPackageDataListScrollYPos) {
                        PackageViwerConf.SetPackageDataListScrollYPos = "";
                        PackageViwerConf.PackageDataListSelectIndex = i;       // 設置選取的 Idx
                        float posY = ImGui::GetCursorPosY() + ImGui::GetFontSize() + 6 - ImGui::GetScrollY() + 80;
                        ImGui::SetScrollFromPosY(posY);
                    }
                })
                    .Show();

                // ========================= Search Bar =========================
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputTextWithHintWithWidth("##PackageDataInput_Result", "Search Package", PackageViwerConf.PackageDataListFilterStr);
                ImGui::PopItemWidth();
    }
    ImGui::EndChild();
}

void PackageViewer::ObjectInfoListRender()
{
    ImGui::SameLine(0, 4);
    if (PackageViwerConf.PackageObjectResizeable) ChildWindowFlag |= ImGuiChildFlags_ResizeX;
    else                                        ChildWindowFlag &= ~ImGuiChildFlags_ResizeX;
    ImGui::BeginChild("PackageObject", ImVec2(PackageViwerConf.PackageObjectWindowWidth, 0), ChildWindowFlag);
    {
        if (ImGui::BeginTabBar("DataListTabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyResizeDown))       // ImGuiTabBarFlags_FittingPolicyScroll(可捲動不壓縮Tab寬度) ImGuiTabBarFlags_FittingPolicyResizeDown(壓縮Tab寬度) | ImGuiTabBarFlags_AutoSelectNewTabs
        {
            const char* names[4] = { "Class", "Struct", "Enum", "Function" };
            for (int n = 0; n < 4; n++) {
                Style::TabStyleSwitch(true);

                // 有 Item 才會顯示 Tab
                GlobalSearchObjectLock.lock();
                if (PackageViwerConf.PackageObjectDataList.find(names[n]) != PackageViwerConf.PackageObjectDataList.end()) {
                    if (ImGui::BeginTabItem(names[n], NULL, PackageViwerConf.PackageObjectDataTabFlagMap[names[n]]))
                    {
                        // Initial Flag
                        std::string TargetList = names[n];
                        if (PackageViwerConf.PackageObjectDataTabFlagMap[names[n]] != ImGuiTabItemFlags_None)
                            PackageViwerConf.PackageObjectDataTabFlagMap[names[n]] = ImGuiTabItemFlags_None;

                        // GUI
                        ImGui::PushFont(Font::TitleText);
                        ImGui::Text("%d %s", PackageViwerConf.PackageObjectDataList[names[n]].size(), names[n]);
                        ImGui::PopFont();

                        // SortOption
                        if (names[n] == "Function") {// Function
                            ImGui::BeginGroupPanel("Option", ImVec2(ImGui::GetContentRegionAvail().x, 0));
                            {
                                ImGui::AlignTextToFramePadding();
                                ImGui::Text("Sort By ");
                                ImGui::SameLine();
                                ImGui::RadioButton("ObjectName", &PackageViwerConf.PackageObjectDataSortMode, PackageViwerConfig::SortMode::Object);
                                ImGui::SameLine();
                                ImGui::RadioButton("FunctName", &PackageViwerConf.PackageObjectDataSortMode, PackageViwerConfig::SortMode::Funct);

                                if (PackageViwerConf.PackageObjectDataSortMode == PackageViwerConfig::SortMode::Object and TargetList == "Function")
                                    TargetList = "FunctionWithObject";
                            }
                            ImGui::EndGroupPanel();
                        }

                        MyListBox("##Package Object", PackageViwerConf.PackageObjectDataListSelectIndex, PackageViwerConf.PackageObjectDataList[TargetList], ImVec2(-FLT_MIN, -FLT_MIN - 30), PackageViwerConf.PackageObjectDataFilterStr)
                            .SetNamePreProcCallBack([&](std::string& Name) {
                            // None
                                })
                            .SetSelectCallBack([&](int Idx, std::pair<const std::string, std::pair<std::string, DWORD_PTR>>& Obj) {
                                    EventHandler::OpenPackageObjectTab(Obj.second.first, Obj.second.second);
                                })
                                    .SetPostCallBack([&](int Idx, std::pair<const std::string, std::pair<std::string, DWORD_PTR>>& Obj) {
                                    if (Obj.second.first == PackageViwerConf.SetObjectDataListScrollYPos) {
                                        PackageViwerConf.SetObjectDataListScrollYPos = "";
                                        PackageViwerConf.PackageObjectDataListSelectIndex = Idx;
                                        float posY = ImGui::GetCursorPosY() + ImGui::GetFontSize() + 6 - ImGui::GetScrollY() + 80;
                                        ImGui::SetScrollFromPosY(posY);
                                    }
                                        })
                                    .Show();
                                        ImGui::EndTabItem();
                    }
                }
                GlobalSearchObjectLock.unlock();
                Style::TabStyleSwitch(false); // 有 tab 才 pop
            }
            ImGui::EndTabBar();
        }

        // ========================= Search Bar =========================
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputTextWithHintWithWidth("##PackageObjectInput_Result", "Search Object", PackageViwerConf.PackageObjectDataFilterStr);
        ImGui::PopItemWidth();
    }
    ImGui::EndChild();
}

void PackageViewer::GlobalSearchListRender()
{
    ImGui::SameLine(0, 4);
    ImGui::BeginChild("GlobalSearchPackageObjectList", ImVec2(PackageViwerConf.GlobalSearchPackageObjectWindowWidth, 0), true);
    {
        ImVec2 ButtonSize = ImVec2(20.f, 100.f);            // X => Width、 Y => Height
        ImVec2 WindowSize = ImGui::GetWindowSize();
        if (WindowSize.x > 30) {
            ImGui::PushFont(Font::TitleText);
            ImGui::Text("Search Result Count : %d", PackageViwerConf.GlobalSearchList.size());
            ImGui::PopFont();
            ImGui::RadioButton("ObjectName", &PackageViwerConf.GlobalSearchMode, PackageViwerConfig::GlobalSearchMode::ObjectName);
            ImGui::SameLine();
            ImGui::RadioButton("MemberName", &PackageViwerConf.GlobalSearchMode, PackageViwerConfig::GlobalSearchMode::MemberName);
            static int LastMode = PackageViwerConfig::GlobalSearchMode::ObjectName;
            if (LastMode != PackageViwerConf.GlobalSearchMode) {
                // 當 Mode 做切換，同樣會觸發搜尋動作
                if (strlen(PackageViwerConf.GlobalSearchStr.c_str()) > 1)
                    EventHandler::UpdataGlobalSearchObject(std::string(PackageViwerConf.GlobalSearchStr));
                else if (PackageViwerConf.GlobalSearchList.size() > 0)
                    PackageViwerConf.GlobalSearchList.clear();
            }
            LastMode = PackageViwerConf.GlobalSearchMode;


            MyListBox("##Search Package Object", PackageViwerConf.GlobalSearchListSelectIndex, PackageViwerConf.GlobalSearchList, ImVec2(-FLT_MIN - ButtonSize.x, -FLT_MIN - 30))
                .SetNamePreProcCallBack([&](std::string& Name) {
                // None
                    })
                .SetSelectCallBack([&](int i, std::pair<const std::string, PackageViwerConfig::GlobalSearchObject>& Obj) {
                        EventHandler::ShowGlobalSearchPackageObjectResult(Obj.second.PackageName, Obj.second.TabName, Obj.second.ObjectName, Obj.second.Address);
                    })
                        .Show();

                    // ========================= Search Bar =========================
                    ImGui::PushItemWidth(-FLT_MIN - ButtonSize.x);
                    ImGui::InputTextWithHintWithWidth("##GlobalSearchObjectInput_Result", "Global Search Object", PackageViwerConf.GlobalSearchStr);
                    if (ImGui::IsItemEdited()) {
                        if (strlen(PackageViwerConf.GlobalSearchStr.c_str()) > 1)
                            // Update 會事先清空 GlobalPackageObject.List 才執行動作 (可查看 UpdataGlobalSearchObject 的行為)
                            EventHandler::UpdataGlobalSearchObject(std::string(PackageViwerConf.GlobalSearchStr));
                        else if (PackageViwerConf.GlobalSearchList.size() > 0)
                            PackageViwerConf.GlobalSearchList.clear();
                    }
                    ImGui::PopItemWidth();

        }

        ImGui::MoveBtn("ObjectInstanceSearch_MoveBtn", WindowSize, &PackageViwerConf.GlobalSearchPackageObjectWindowWidth, ButtonSize);
    }
    ImGui::EndChild();
}

void PackageViewer::ObjectContentRender()
{
    static float CurPanelWindow_Width = ImGui::GetWindowSize().x;
    ImGui::SameLine();
    ImGui::BeginChild("PackageObjectContent", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
        if (ImGui::BeginTabBar("PackageObjectTabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_AutoSelectNewTabs))
        {
            for (int n = 0; n < PackageViwerConf.ObjectContentTabNameList.size(); n++) {
                Style::TabStyleSwitch(true);

                //先檢查有沒有該 Object，有才顯示
                const float FrameHeight = ImGui::CalcTextSize("Inheritance", NULL, true).y;
                DumperObject PackObject;
                if (PackageViwerConf.GetPackageObjectByName(PackageViwerConf.ObjectContentTabNameList[n], PackObject)) {
                    bool IsTabOpen = PackageViwerConf.ObjectContentTabOpenState[PackObject.Address];    // bool 比較麻煩，因為 compiler 為了做優化會做 bit mask (1 bytes 可以存 8 個 bool)，所以會沒辦法 reference
                    if (IsTabOpen and ImGui::BeginTabItem(PackageViwerConf.ObjectContentTabNameList[n].c_str(), &IsTabOpen, ImGuiTabItemFlags_None))
                    {
                        ImGui::PushFont(Font::TitleText);
                        std::string ObjectType = Utils.rStringToLower(PackObject.Type);    //小寫 => 不分大小寫匹配
                        // =========================  Title : Object Info  =========================
                        {
                            ImGui::Text("Address: ");
                            ImGui::SameLine();
                            ImGui::CopyBtn("ObjectInfo_" + PackObject.Name + "_" + PackObject.Type, Utils.HexToString(PackObject.Address));
                            ImGui::SameLine(0, 0);
                            ImGui::Text("0x%p", PackObject.Address);
                            if (ObjectType.find("function") != std::string::npos) {
                                ImGui::Text("Function Address: "); ImGui::SameLine();
                                ImGui::CopyBtn("FunctionAddress_" + PackObject.Name + "_" + PackObject.Type, Utils.HexToString(PackObject.Funct.Address)); ImGui::SameLine(0, 0);
                                ImGui::Text("0x%p", PackObject.Funct.Address); ImGui::SameLine(); ImGui::Text("("); ImGui::SameLine();
                                ImGui::Text("Offset: "); ImGui::SameLine();
                                ImGui::CopyBtn("FunctionAddressOffset_" + PackObject.Name + "_" + PackObject.Type, Utils.HexToString(PackObject.Funct.Offset)); ImGui::SameLine(0, 0);
                                ImGui::Text("0x%llX", PackObject.Funct.Offset); ImGui::SameLine();  ImGui::Text(")");
                            }
                            PackObject.ObjectAddress == NULL ? ImGui::Text("Object Pointer: NULL") : ImGui::Text("Object Pointer: 0x%p", PackObject.ObjectAddress); ;
                            ImGui::Text("Type: %s", PackObject.Type.c_str());
                            ImGui::Text("Name: %s", PackObject.Name.c_str());
                            ImGui::Text("FullName: %s", PackObject.FullName.c_str());

                            // Class 繼承 Chain
                            ImGui::Text("Inheritance:");
                            ImGui::SameLine();
                            Style::TransparentButtonStyleSwitch(true, Color::ObjectClickableColor);
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                            ImGui::BeginChild(std::string("##InheritanceChain_MyObjectName").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, FrameHeight), false);
                            for (int i = (int)PackObject.Super.size() - 1; i >= 0; i--)
                            {
                                ImGui::PushID(i);
                                ClickableObjectBtn(PackObject.Super[i].Name, PackObject.Super[i].Address);
                                ImGui::PopID();
                                ImGui::SameLine(); ImGui::Text(">"); ImGui::SameLine();
                            }
                            if (ImGui::Button((PackObject.Name + "##" + PackObject.Name + "_" + std::to_string(PackObject.Super.size())).c_str())) {};//這個 Obj 就是當前在看的 => 不動作
                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                            ImGui::PopStyleVar(1);
                            Style::TransparentButtonStyleSwitch(false);
                            ImGui::EndChild();
                        }



                        // =========================  Object Body  =========================
                        Style::TransparentButtonStyleSwitch(true, Color::ObjectClickableColor);
                        ImGui::Dummy(ImVec2(0.0f, 10.0f));
                        {
                            // Basic
                            ImGui::TextColored(Color::TypeColor, PackObject.Type.c_str());
                            ImGui::SameLine();
                            ImGui::TextColored(Color::NameColor, PackObject.Name.c_str());


                            // Is Class or Struct
                            if (ObjectType.find("class") != std::string::npos or ObjectType.find("objectproperty") != std::string::npos or ObjectType.find("struct") != std::string::npos)
                            {
                                if (!PackObject.Super.empty()) {
                                    ImGui::SameLine();
                                    ImGui::Text(":");
                                    ImGui::SameLine();
                                    ClickableObjectBtn(PackObject.Super[0].Name, PackObject.Super[0].Address);
                                }

                                SearchMatchCnt = 0; // 歸零
                                for (int i = 0; i < PackObject.Member.size(); i++)
                                {
                                    // Type
                                    // MemberList[i] Type 是不是也是一個物件，如果是則以按鈕顯示，使用者點擊後可開啟對應物件的分頁
                                    ImGui::SetCursorPosX(PackageViwerConf.ObjectTypeIndentation);
                                    if (PackObject.Member[i].Clickable) {
                                        ImGui::PushID(i);
                                        ClickableObjectBtn(PackObject.Member[i].Type, PackObject.Member[i].Address);
                                        ImGui::PopID();
                                    }
                                    else ImGui::TextColored(Color::TypeColor, PackObject.Member[i].Type.c_str());

                                    //SubType
                                    if (PackObject.Member[i].SubType.size() > 0) {
                                        ImGui::SameLine(); ImGui::Text("<"); ImGui::SameLine();
                                        for (int j = 0; j < PackObject.Member[i].SubType.size(); j++) {
                                            if (j >= 1) { ImGui::SameLine(); ImGui::Text(","); ImGui::SameLine(); }
                                            if (PackObject.Member[i].SubType[j].Clickable) {
                                                ImGui::PushID(i);
                                                ClickableObjectBtn(PackObject.Member[i].SubType[j].Name, PackObject.Member[i].SubType[j].Address);
                                                ImGui::PopID();
                                            }
                                            else ImGui::TextColored(Color::TypeColor, PackObject.Member[i].SubType[j].Name.c_str());
                                        }
                                        ImGui::SameLine(); ImGui::Text(">"); ImGui::SameLine();
                                    }

                                    //Name
                                    ImGui::SameLine();
                                    ImGui::SetCursorPosX(PackageViwerConf.ObjectNameindentation);
                                    ImGui::TextColored(Color::NameColor, PackObject.Member[i].Name.c_str());

                                    // Highlight (If SerchStr Equal With)
                                    ImGui::HighlightStringChecker(PackObject.Member[i].Name, PackageViwerConf.ObjectContentSearchStr, Color::HighlightColor, &SearchMatchCnt);
                                    if (SearchFocus and PackageViwerConf.ObjectContentSerchIdx == SearchMatchCnt) {
                                        float posY = ImGui::GetCursorPosY() + ImGui::GetFontSize() - ImGui::GetScrollY();
                                        ImGui::SetScrollFromPosY(posY);
                                        SearchFocus = false;
                                    }
                                }
                            }

                            // Is Enum
                            else if (ObjectType.find("enum") != std::string::npos)
                            {
                                ImGui::SameLine();
                                ImGui::Text(":");
                                ImGui::SameLine();
                                ImGui::TextColored(Color::TypeColor, (PackObject.TypeObject.empty() ? "Byte" : PackObject.TypeObject.c_str()));

                                for (int i = 0; i < PackObject.Enum.size(); i++)
                                {
                                    ImGui::SetCursorPosX(PackageViwerConf.ObjectTypeIndentation);
                                    ImGui::TextColored(Color::TypeColor, PackObject.Enum[i].first.c_str());
                                    ImGui::SameLine();
                                    ImGui::SetCursorPosX(PackageViwerConf.ObjectNameindentation);
                                    ImGui::TextColored(Color::ValueColor, std::to_string(PackObject.Enum[i].second).c_str());
                                }
                            }

                            // Is Function
                            else if (ObjectType.find("function") != std::string::npos)
                            {
                                // Basic
                                if (PackObject.Funct.Ret.Name == "Void")
                                    ImGui::TextColored(Color::TypeColor, "Void");
                                else if (PackObject.Funct.Ret.SubType.size() == 0)
                                    ImGui::TextColored(Color::TypeColor, PackObject.Funct.Ret.Type.c_str());
                                else if (PackObject.Funct.Ret.SubType[0].Clickable)
                                    ClickableObjectBtn(PackObject.Funct.Ret.SubType[0].Name, PackObject.Funct.Ret.SubType[0].Address);
                                else
                                    ImGui::TextColored(Color::TypeColor, PackObject.Funct.Ret.SubType[0].Name.c_str());
                                ImGui::SameLine();
                                ClickableObjectBtn(PackObject.Funct.Outer.Name, PackObject.Funct.Outer.Address);
                                ImGui::SameLine(0, 0);
                                ImGui::TextColored(Color::White, "::");
                                ImGui::SameLine(0, 0);
                                float CursorPoxX = ImGui::GetCursorPosX();
                                ImGui::TextColored(Color::NameColor, PackObject.Name.c_str());
                                ImGui::SameLine();
                                ImGui::TextColored(Color::White, " ( ");
                                if (PackObject.Funct.Para.size() == 0) {
                                    ImGui::SameLine();
                                    ImGui::TextColored(Color::White, " ) ");
                                }
                                else {
                                    for (int i = 0; i < PackObject.Funct.Para.size(); i++) {
                                        ImGui::SetCursorPosX(CursorPoxX + 10);
                                        if (PackObject.Funct.Para[i].SubType.size() == 0)
                                            ImGui::TextColored(Color::TypeColor, PackObject.Funct.Para[i].Type.c_str());
                                        else if (PackObject.Funct.Para[i].SubType[0].Clickable)
                                            ClickableObjectBtn(PackObject.Funct.Para[i].SubType[0].Name, PackObject.Funct.Para[i].SubType[0].Address);
                                        else ImGui::TextColored(Color::TypeColor, PackObject.Funct.Para[i].SubType[0].Name.c_str());
                                        ImGui::SameLine();
                                        ImGui::TextColored(Color::NameColor, PackObject.Funct.Para[i].Name.c_str());
                                    }
                                    ImGui::SetCursorPosX(CursorPoxX - 1);
                                    ImGui::TextColored(Color::White, " ) ");
                                }
                            }
                        }
                        Style::TransparentButtonStyleSwitch(false);
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                    // 刪除該 Tab
                    if (IsTabOpen == false) {
                        // 從 OpenState 紀錄中刪除
                        PackageViwerConf.ObjectContentTabOpenState.erase(PackObject.Address);

                        // 刪除對應 Tab 資料
                        PackageViwerConf.ObjectContentMap.erase(PackageViwerConf.ObjectContentTabNameList[n]);        // PackageObjectMap 中刪除該物件
                        PackageViwerConf.ObjectContentTabNameList.erase(PackageViwerConf.ObjectContentTabNameList.begin() + n); // TabNameList 刪除中刪除該 Nme 對應的 Tab
                    }
                }
                Style::TabStyleSwitch(false);
            }
            ImGui::EndTabBar();
        }
        // =========================  Search Bar  =========================
        static MyInputBarBtn::StateStruct State = MyInputBarBtn::StateStruct::Close;
        static float Timer = 1;
        static ImVec2 Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, ImGui::GetScrollY() + 30);
        Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, (ImGui::GetScrollY() > 30 ? ImGui::GetScrollY() + 10 : 40));
        MyInputBarBtn("PackageObjectContentSearchInput", "Search Member", Timer, 1, 220, PackageViwerConf.ObjectContentSearchStr, PackageViwerConf.ObjectContentSerchIdx, SearchMatchCnt, SearchFocus, State)
            .SetPos(Pos).SetFont(Font::IconText).SetSpeed(20.f).Show();
    }
    ImGui::EndChild();
}
