#pragma once
#include <string>
#include <shared_mutex>
#include "../../../imgui/imgui.h"
#include "../../MyGuiComponent.h"
#include "../../MyGuiUtils.h"
#include "../../Interface/DumperConsoleInterface.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../../State/EventHandler.h"

namespace Console
{
    std::shared_mutex DumperTableLock;

    void DisplayDumperObject(DumperItem& DumperItem, DumperObject& DumperObject, size_t Offset = 0x0);
    void DisplayDumperObjectEntry(DumperItem& DumperItem);

    void ObjectDataRender();
    void ObjectInfoRender();

    void Render(DumperConsoleCurPage& PageID) {
        if (PageID != DumperConsoleCurPage::Console) return;
        ObjectDataRender();
        ObjectInfoRender();
    }
}

// ============================================================================================
//                                  Page Render
// ============================================================================================

void Console::ObjectDataRender()
{
    ImGui::BeginChild("ObjectData", ImVec2(0 - MainConsoleConf.ObjectInfoWindoWidth, 0), true);      //設成 0 就會自動伸縮
    {
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Object Data");
        ImGui::PopFont();

        for (int n = 0; n < MainConsoleConf.DumperTable.size(); n++) {
            ImGui::PushID(n);
            DumperTableLock.lock();
            std::string UniqueID = "[Table]  " + Utils.HexToString(MainConsoleConf.DumperTable[n].Address) + " - " + MainConsoleConf.DumperTable[n].Name;
            if (ImGui::CollapsingHeader(UniqueID.c_str(), &MainConsoleConf.DumperTable[n].IsExist, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
            {

                const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
                const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
                static ImGuiTableFlags TableFlags =
                    ImGuiTableFlags_BordersV |          //最外圈垂直的線
                    ImGuiTableFlags_BordersOuterH |     //最外圈水平的線
                    ImGuiTableFlags_Resizable |
                    ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Hideable |      //可隱藏 column
                    ImGuiTableFlags_ScrollX |       //X 軸可滾動
                    ImGuiTableFlags_ScrollY |       //Y 軸可滾動
                    ImGuiTableFlags_NoBordersInBody;

                if (ImGui::BeginTable(("ObjectTable" + std::to_string(n)).c_str(), 4, TableFlags, ImVec2(MainConsoleConf.DumperTableWidth, MainConsoleConf.DumperTableHeight)))           //colum 調整時這邊要調整
                {
                    ImGui::TableSetupScrollFreeze(1, 1);    //固定顯示開頭第一 column、row(照順序)

                    ImGui::TableSetupColumn("Offset - Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Struct", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                    ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
                    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
                    ImGui::TableHeadersRow();


                    if (MainConsoleConf.DumperTable[n].ObjectAddress)
                        DisplayDumperObjectEntry(MainConsoleConf.DumperTable[n]);

                    ImGui::EndTable();
                }
            }


            // drag sources and drag targets here!
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("DumperItem_Drag", &n, sizeof(int));

                    // 抓取時顯示的訊息
                    ImGui::Text("Exchange %X", MainConsoleConf.DumperTable[n].Address);
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DumperItem_Drag"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        {
                            printf("[ Exchange %d %d ]\n", payload_n, n);
                            DumperItem TempDumperItem;
                            TempDumperItem = MainConsoleConf.DumperTable[n];
                            MainConsoleConf.DumperTable[n] = MainConsoleConf.DumperTable[payload_n];
                            MainConsoleConf.DumperTable[payload_n] = TempDumperItem;

                            int Idx;
                            Idx = MainConsoleConf.DumperTable[n].Index;
                            MainConsoleConf.DumperTable[n].Index = MainConsoleConf.DumperTable[payload_n].Index;
                            MainConsoleConf.DumperTable[payload_n].Index = Idx;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            //如果關閉，就刪除
            if (!MainConsoleConf.DumperTable[n].IsExist) {
                for (int i = n + 1; i < MainConsoleConf.DumperTable.size(); i++) MainConsoleConf.DumperTable[i].Index -= 1;
                MainConsoleConf.DumperTable.erase(MainConsoleConf.DumperTable.begin() + n);
            }
            DumperTableLock.unlock();
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}

void Console::ObjectInfoRender()
{
    ImGui::SameLine();
    ImGui::BeginChild("ObjectInfo", ImVec2(MainConsoleConf.ObjectInfoWindoWidth, 0), true);
    {
        // ==================== Setting ====================
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Console Setting");
        ImGui::PopFont();

        ImGui::DummySpace();
        ImGui::DragFloatWithWidth("Object Info Width", MainConsoleConf.ObjectInfoWindoWidth, MainConsoleConf.DragAreaWidth);

        ImGui::DummySpace();
        ImGui::DragFloatWithWidth("Setting DragArea Width", MainConsoleConf.DragAreaWidth, MainConsoleConf.DragAreaWidth);

        ImGui::DummySpace();
        ImGui::DragFloatWithWidth("Input Area Width", MainConsoleConf.InputTextWidth, MainConsoleConf.DragAreaWidth);

        ImGui::DummySpace();
        ImGui::DragFloatWithWidth("Dumper TB Width", MainConsoleConf.DumperTableWidth, MainConsoleConf.DragAreaWidth);

        ImGui::DummySpace();
        ImGui::DragFloatWithWidth("Dumper TB Height", MainConsoleConf.DumperTableHeight, MainConsoleConf.DragAreaWidth);

        // ==================== Option ====================
        ImGui::SeparatorText("");
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Object Info");
        ImGui::PopFont();

        ImGui::DummySpace();
        {
            if (ImGui::Button("Get FName")) {
                EventHandler::GetFName();
            }
            ImGui::SameLine();
            ImGui::PushItemWidth(MainConsoleConf.InputTextWidth - 90.0f);
            ImGui::InputTextWithHintWithWidth("##GetFNameByIDInput_Result", "Enter Object ID", MainConsoleConf.FNameInfo.InputStr);
            ImGui::PopItemWidth();
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("##GetFNameByIDOutput_Result", MainConsoleConf.FNameInfo.FName, MainConsoleConf.InputTextWidth);
        }

        ImGui::DummySpace();
        {
            MyComboList("GetGUObjectMode", MainConsoleConf.GUObjectInfo.ModeListSelectIndex, MainConsoleConf.GUObjectInfo.ModeList, ImGuiComboFlags_NoPreview)
                .Show();

            // Get Object
            ImGui::SameLine(0, (float)0.1);
            const float FrameHeight = GUIUtils.GetFrameHeight();
            if (ImGui::Button("Get GUObject")) {
                EventHandler::GetGUObject();
            }
            //AddObject
            ImGui::SameLine();
            {
                ImGui::PushFont(Font::IconText);
                if (ImGui::Button(MainConsoleConf.AddButtonStr.c_str(), ImVec2(MainConsoleConf.AddButtonWidth, FrameHeight))) {       //"Add Object"
                    EventHandler::AddToObjectTable();
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) { MainConsoleConf.AddButtonIsHovered = true; MainConsoleConf.AddButtonStr = ICON_FA_CIRCLE_ARROW_DOWN " Save"; }
                else { MainConsoleConf.AddButtonIsHovered = false; MainConsoleConf.AddButtonStr = ICON_FA_CIRCLE_ARROW_DOWN; }
                ImGui::PopFont();
            }
            // Parseing 
            ImGui::SameLine();
            {
                ImGui::PushFont(Font::IconText);
                if (ImGui::Button(MainConsoleConf.ParseButtonStr.c_str(), ImVec2(MainConsoleConf.ParseButtonWidth, FrameHeight))) {       //"Add Object"
                    EventHandler::ParseObject();
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) { MainConsoleConf.ParseButtonIsHovered = true; MainConsoleConf.ParseButtonStr = ICON_FA_FILE_MEDICAL " Parse"; }
                else { MainConsoleConf.ParseButtonIsHovered = false; MainConsoleConf.ParseButtonStr = ICON_FA_FILE_MEDICAL; }
                ImGui::PopFont();
            }
            // Input
            ImGui::SameLine();
            ImGui::PushItemWidth(MainConsoleConf.InputTextWidth - 190.0f);
            ImGui::InputTextWithHintWithWidth("##GetGUObjectByAddress_Result", ("Enter Object " + MainConsoleConf.GUObjectInfo.ModeList[MainConsoleConf.GUObjectInfo.ModeListSelectIndex]).c_str(), MainConsoleConf.GUObjectInfo.InputStr);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (MainConsoleState.GUObjectInfoParseEvent == ProcessState::Processing) {
                const ImU32 col = ImGui::ColorConvertFloat4ToU32(Color::SpinnerColor);
                ImGui::Spinner("##spinner", 10, 6, col);
            }
            //上面的 Button Animation
            ImTween<float>::Tween(std::tuple{ MainConsoleConf.AddButtonInitialWidth, 60.0f, &MainConsoleConf.AddButtonWidth })
                .OfType(ImTween<>::TweenType::PingPong)
                .Speed(8.0f)
                .When([&]() { return MainConsoleConf.AddButtonIsHovered; })
                .OnComplete([&]() {})
                .Tick();
            //上面的 Button Animation
            ImTween<float>::Tween(std::tuple{ MainConsoleConf.ParseButtonInitialWidth, 60.0f, &MainConsoleConf.ParseButtonWidth })
                .OfType(ImTween<>::TweenType::PingPong)
                .Speed(8.0f)
                .When([&]() { return MainConsoleConf.ParseButtonIsHovered; })
                .OnComplete([&]() {})
                .Tick();
        }

        {
            // ID
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("ID", MainConsoleConf.GUObjectInfo.ID, MainConsoleConf.InputTextWidth);

            //Type
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Type", MainConsoleConf.GUObjectInfo.Type, MainConsoleConf.InputTextWidth);

            //Name
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Name", MainConsoleConf.GUObjectInfo.Name, MainConsoleConf.InputTextWidth);

            //FullName
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("FullName", MainConsoleConf.GUObjectInfo.FullName, MainConsoleConf.InputTextWidth);

            //Address
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Address", MainConsoleConf.GUObjectInfo.Address, MainConsoleConf.InputTextWidth);

            //Offset
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Offset", MainConsoleConf.GUObjectInfo.Offset, MainConsoleConf.InputTextWidth);

            //ClassPtr
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("ClassPtr", MainConsoleConf.GUObjectInfo.ClassPtr, MainConsoleConf.InputTextWidth);

            //Funct
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Funct", MainConsoleConf.GUObjectInfo.Funct, MainConsoleConf.InputTextWidth);

            //Outer
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Outer", MainConsoleConf.GUObjectInfo.Outer, MainConsoleConf.InputTextWidth);

            //SuperPtr
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("SuperPtr", MainConsoleConf.GUObjectInfo.SuperPtr, MainConsoleConf.InputTextWidth);

            //Propsize
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("Propsize", MainConsoleConf.GUObjectInfo.PropSize, MainConsoleConf.InputTextWidth);

            //Property
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("PropPtr 0", MainConsoleConf.GUObjectInfo.PropPtr_0, MainConsoleConf.InputTextWidth);

            ImGui::DummySpace();
            ImGui::InputTextWithWidth("PropPtr 1", MainConsoleConf.GUObjectInfo.PropPtr_1, MainConsoleConf.InputTextWidth);

            //MemberSize
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("MemberSize", MainConsoleConf.GUObjectInfo.MemberSize, MainConsoleConf.InputTextWidth);

            ImGui::DummySpace();
            ImGui::InputTextWithWidth("MemberPtr 0", MainConsoleConf.GUObjectInfo.MemberPtr_0, MainConsoleConf.InputTextWidth);

            //BitMask
            ImGui::DummySpace();
            ImGui::InputTextWithWidth("BitMask", MainConsoleConf.GUObjectInfo.Bitmask, MainConsoleConf.InputTextWidth);
        }
    }
    ImGui::EndChild();
}

// ============================================================================================
//                                  Dynamic Page Render 
// ============================================================================================

void Console::DisplayDumperObject(DumperItem& DumperItem, DumperObject& DumperObject, size_t Offset)
{
    for (BasicDumperObject& MemberObject : DumperObject.Member)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // Select Setting
        bool IsSelected = false;
        if (DumperItem.SelectedList.contains(MemberObject.Address)) IsSelected = true;
        if (ImGui::Selectable(("##" + MemberObject.Name).c_str(), IsSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 0.0f)))
        {
            if (ImGui::GetIO().KeyCtrl) {
                if (IsSelected)     DumperItem.SelectedList.find_erase_unsorted(MemberObject.Address);    //選擇的狀況下在按一次就取消
                else                DumperItem.SelectedList.push_back(MemberObject.Address);
            }
            else {
                DumperItem.SelectedList.clear();
                DumperItem.SelectedList.push_back(MemberObject.Address);
            }
        }
        ImGui::SameLine();

        if (GUIUtils.IsExpandType(MemberObject.Type))
        {
            size_t NextOffest = 0;
            if (MemberObject.Type.find("ObjectProperty") != std::string::npos or
                MemberObject.Type.find("ClassProperty") != std::string::npos)
                NextOffest = 0;
            else NextOffest = MemberObject.Offset + Offset;


            //Header
            std::string HeaderStr = Utils.HexToString(MemberObject.Offset + Offset) + " - " + MemberObject.Name;
            bool Open = ImGui::TreeNodeEx(HeaderStr.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
            //Struct
            ImGui::TableNextColumn();
            std::string StructStr = "[ Pointer ] : " + MemberObject.Type;
            ImGui::Text(StructStr.c_str());
            //Address
            ImGui::TableNextColumn();
            std::string AddressStr = Utils.HexToString(MemberObject.Address);
            ImGui::Text(AddressStr.c_str());
            //Value
            ImGui::TableNextColumn();
            std::string ValueStr = Utils.HexToString(MemberObject.Value);
            char ValueStrBuf[128];
            strcpy_s(ValueStrBuf, sizeof(ValueStrBuf), ValueStr.c_str());
            ImGui::InputText(("##DumperItem_MemberObject_" + Utils.HexToString(MemberObject.Address)).c_str(), ValueStrBuf, IM_ARRAYSIZE(ValueStrBuf));
            if (Open)
            {
                if (DumperItem.ObjectMap.find(MemberObject.Address) == DumperItem.ObjectMap.end())
                    EventHandler::UpdateDumperObjectMap(MemberObject.Address, DumperItem.Index);
                else
                    DisplayDumperObject(DumperItem, DumperItem.ObjectMap[MemberObject.Address], NextOffest);
                ImGui::TreePop();
            }
        }
        else
        {
            //Header
            std::string BitStr = MemberObject.Bit != -1 ? ":" + std::to_string(MemberObject.Bit) : "";
            std::string HeaderStr = Utils.HexToString(MemberObject.Offset + Offset) + BitStr + " - " + MemberObject.Name;
            ImGui::BulletText(HeaderStr.c_str());
            //Struct
            ImGui::TableNextColumn();
            std::string StructStr = "[ Value ] : " + MemberObject.Type;
            ImGui::Text(StructStr.c_str());
            //Address
            ImGui::TableNextColumn();
            std::string AddressStr = Utils.HexToString(MemberObject.Address);
            ImGui::Text(AddressStr.c_str());
            //Value
            ImGui::TableNextColumn();
            std::string ValueStr = Utils.HexToString(MemberObject.Value);
            char ValueStrBuf[128];
            strcpy_s(ValueStrBuf, sizeof(ValueStrBuf), ValueStr.c_str());
            ImGui::InputText(("##DumperItem_MemberObject_" + Utils.HexToString(MemberObject.Address)).c_str(), ValueStrBuf, IM_ARRAYSIZE(ValueStrBuf));
        }
    }
}

void Console::DisplayDumperObjectEntry(DumperItem& DumperItem)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    // Select Setting
    bool IsSelected = false;
    if (DumperItem.SelectedList.contains(DumperItem.Address)) IsSelected = true;
    if (ImGui::Selectable(("##" + DumperItem.Name).c_str(), IsSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 0.0f)))
    {
        if (ImGui::GetIO().KeyCtrl) {
            if (IsSelected)     DumperItem.SelectedList.find_erase_unsorted(DumperItem.Address);    //選擇的狀況下在按一次就取消
            else                DumperItem.SelectedList.push_back(DumperItem.Address);
        }
        else {
            DumperItem.SelectedList.clear();
            DumperItem.SelectedList.push_back(DumperItem.Address);
        }
    }
    ImGui::SameLine();
    //Header
    std::string HeaderStr = DumperItem.Name;
    bool Open = ImGui::TreeNodeEx(HeaderStr.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
    //Struct
    ImGui::TableNextColumn();
    std::string StructStr = "[ Pointer ] : " + DumperItem.Type;
    ImGui::Text(StructStr.c_str());
    //Address
    ImGui::TableNextColumn();
    std::string AddressStr = Utils.HexToString(DumperItem.Address);
    ImGui::Text(AddressStr.c_str());
    //Value
    ImGui::TableNextColumn();
    std::string ValueStr = Utils.HexToString(DumperItem.Value);
    char ValueStrBuf[128];
    strcpy_s(ValueStrBuf, sizeof(ValueStrBuf), ValueStr.c_str());
    ImGui::InputText(("##DumperItem_" + Utils.HexToString(DumperItem.Address)).c_str(), ValueStrBuf, IM_ARRAYSIZE(ValueStrBuf));
    if (Open)
    {
        if (DumperItem.ObjectMap.find(DumperItem.Address) == DumperItem.ObjectMap.end())
            EventHandler::UpdateDumperObjectMap(DumperItem.Address, DumperItem.Index);
        else
            DisplayDumperObject(DumperItem, DumperItem.ObjectMap[DumperItem.Address]);
        ImGui::TreePop();
    }
}


