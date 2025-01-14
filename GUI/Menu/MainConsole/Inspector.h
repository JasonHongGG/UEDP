#pragma once
#include <string>
#include <Windows.h>
#include <shared_mutex>
#include "../../../imgui/imgui.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"
#include "../../Config/StyleConfig.h"
#include "../../Config/ToggleConfig.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../Config/TimerConfig.h"
#include "../../MyGuiComponent.h"
#include "../../MyGuiUtils.h"
#include "../../../State/EventHandler.h"
#include "../../Interface/DumperConsoleInterface.h"
#include "../../../System/Memory.h"

namespace Inspector
{
    inline TimerConfig::TimerObj& TimeObjRef = TimerConf.TimeObjMap["InspectorMemoryScanTimer"];          // �j�M�ɶ�
    inline std::shared_mutex GetInspectorObjectContentMapLock;
    inline std::shared_mutex GetObjectInstanceSearchLock;
    inline std::shared_mutex GetObjectContentMapLock;
    inline bool InspectorListResizeable = true;
    inline ImGuiChildFlags ChildWindowFlag = ImGuiChildFlags_Border;
    inline float InspectorListWindowWidth = 200;
    inline float ObjectInstanceSearchWindowWidth = 20;
    inline int SearchMatchCnt = 0;
    inline bool SearchFocus = false;

    int CalcuNextMapItemOffset(DWORD_PTR Address, size_t MemberSize);
    void DisplayInspectorObject(DumperObject& InspectorSuperObject, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath, bool EditorEnable, DWORD_PTR Address, size_t Offset, float Indentation);
    void DisplayInspectorObjectEntry(std::vector<DumperObject>& InspectorObjectSuperList, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath, bool EditorEnable = false, bool ShowHeader = true, DWORD_PTR Address = NULL, size_t Offest = 0x0, float Indentation = 0);


    void InspectorList();
    void DerivedList();
    void ConditionPage(int Idx, InspectorConfig::ConditionObject& Condition);
    void InstanceSearchList();
    void InspectorObjectContent();

    void Render(DumperConsoleCurPage& PageID) {
        if (PageID != DumperConsoleCurPage::Inspector) return;

        InspectorList();
        DerivedList();
        InstanceSearchList();

        ImGui::GradientComponent(true);
        InspectorObjectContent();
        ImGui::GradientComponent(false, Color::GradientColor1, Color::GradientColor2, ImGui::GradientMode::Horizontal);
    }
}

int Inspector::CalcuNextMapItemOffset(DWORD_PTR Address, size_t MemberSize)
{
    // �p��� Map �� Item ������ Offset
    // �D�n�O Map ���� Obj �i�H�O�����i�}���A�ɭP�� item �����u�t ProcessInfo::ProcOffestAdd

    int MaxLevel = 20;  // �u���@�� Item �N�L�k�p��A�]��������קK�N�~
    int Offset = 0;
    int RetOffset = 0;
    for (int i = 0; i < MaxLevel; i++) {
        Offset = i * (int)ProcessInfo::ProcOffestAdd;
        if (MemMgr.MemReader.ReadMem<int>(Address + Offset) == 0xFFFFFFFF        // 0xFFFFFFFF ��ܵ����A�γo�Ӥ覡�b�O����W�����
            //and MemMgr.ReadMem<int>(Address + Offset + 0x4).first <= MemberSize     // Map Item ID �| < MemberSize
            )
        {
            RetOffset = Offset + (int)ProcessInfo::ProcOffestAdd;
            break;
        }
    }
    return RetOffset;
}

void Inspector::DisplayInspectorObject(DumperObject& InspectorSuperObject, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath, bool EditorEnable, DWORD_PTR Address, size_t Offset, float Indentation)
{

    int MapAdditionOffset = -1;
    int MemberIdx = 0;
    for (BasicDumperObject& MemberObject : InspectorSuperObject.Member)
    {
        MemberIdx++;

        if (MapAdditionOffset == -1 and EditorEnable and MemberObject.Index == 1 and MemberIdx > 1) // �b��ܲĤG������ɷ|���h�p�⦹ map ����� Item ������ Offset
            MapAdditionOffset = CalcuNextMapItemOffset(Address + Offset, InspectorSuperObject.Member.size());

        float IndentAccum = Indentation;
        int CurOffset = (int)Offset + (MapAdditionOffset == -1 ? (int)MemberObject.Offset : MemberObject.Index * MapAdditionOffset);    //�A�Ω� struct�Barray�Bmap ���D���Ы��V�s�Ŷ������c
        if (InspectorSuperObject.Type == "MapProperty") {
            // Is Value
            if (MemberIdx % 2 == 0) {
                CurOffset + 0x8;            // TODO: ���ӭn�ھ� Key ���j�p�M�w offset
            }
        }

        // Type Name    // �p�G�O Property ������A�h���̭����u����W�٧@����ܪ� Type Name
        std::string TargetObjectName = MemberObject.Type;   // Default is Object Type => mean no TargetObjectName
        GUIUtils.GetObjectName(MemberObject, TargetObjectName, EditorEnable);



        // =====================  Display  =====================
        // Offset
        std::string BitStr = MemberObject.Bit != -1 ? ":" + std::to_string(MemberObject.Bit) : "";
        ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(InspectorConf.InspectorOffsetIndentation, Indentation, IndentAccum, true, true));
        int NavLineIndentSize = ImGui::GetCursorPos().x;
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(Color::OffestColor, ("+" + Utils.HexToString(CurOffset) + BitStr).c_str());
        IndentAccum += GUIUtils.GetStringWidth("+" + Utils.HexToString(CurOffset) + BitStr) + InspectorConf.AdditionIndentWidth;

        //Type
        ImGui::SameLine();
        ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(InspectorConf.InspectorTypeIndentation, Indentation, IndentAccum, false, false, NavLineIndentSize + GUIUtils.GetStringWidth("+CCC:8")));
        Style::TreeFrameStyleSwitch(true);
        Style::TreeNodeHeaderStyleSwitch(true);
        bool Open = false;
        if (GUIUtils.IsExpandType(MemberObject.Type)) {
            // �i�i�}�� tree node
            Open = ImGui::TreeNodeEx((TargetObjectName + "##" + MemberObject.Name + "_" + Utils.HexToString(Address + CurOffset)).c_str(), MainConsoleConf.TreeNodeExFlag);
        }
        else {
            // leaf node
            ImGui::PushStyleColor(ImGuiCol_Text, Color::TypeColor);
            ImGui::TreeNodeEx((TargetObjectName + "##" + MemberObject.Name + "_" + Utils.HexToString(Address + CurOffset)).c_str(), MainConsoleConf.TreeNodeExLeafFlag);
            ImGui::PopStyleColor();
        }
        Style::TreeNodeHeaderStyleSwitch(false);
        Style::TreeFrameStyleSwitch(false);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        IndentAccum += GUIUtils.GetStringWidth(TargetObjectName) + InspectorConf.AdditionIndentWidth;

        // Name
        ImGui::SameLine();
        ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(InspectorConf.InspectorNameIndentation, Indentation, IndentAccum));
        ImGui::TextColored(Color::NameColor, MemberObject.Name.c_str());
        IndentAccum += GUIUtils.GetStringWidth(MemberObject.Name);
        // Hightlight
        ImGui::HighlightStringChecker(MemberObject.Name, InspectorConf.InspectorObjectContentSearchStr, Color::HighlightColor, &SearchMatchCnt);
        if (SearchFocus and InspectorConf.InspectorObjectContentSearchIdx == SearchMatchCnt) {
            float posY = ImGui::GetCursorPosY() + ImGui::GetFontSize() - ImGui::GetScrollY();
            ImGui::SetScrollFromPosY(posY);
            SearchFocus = false;
        }

        // Object Address
        ImGui::SameLine();
        ImGui::CopyBtn(MemberObject.Name, Utils.HexToString(MemberObject.ObjectAddress), GUIUtils.CursorPosCalcu(InspectorConf.InspectorObjectAddressIndentation, Indentation, IndentAccum, true), Style::TreeNodePadding);
        ImGui::SameLine(0, 0);
        ImGui::TextColored(Color::ObjectAddressColor, Utils.HexToString(MemberObject.ObjectAddress).c_str());

        if (EditorEnable)
        {
            //Address
            ImGui::SameLine();
            ImGui::CopyBtn(MemberObject.Name + MemberObject.Type + "_EditorEnable", Utils.HexToString(Address + CurOffset), GUIUtils.CursorPosCalcu(InspectorConf.InspectorAddressIndentation, Indentation, IndentAccum), Style::TreeNodePadding);
            ImGui::SameLine(0, 0);
            ImGui::TextColored(Color::AddressColor, Utils.HexToString(Address + CurOffset).c_str());

            //Value
            ImGui::SameLine();
            ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(InspectorConf.InspectorValueIndentation, Indentation, IndentAccum));
            std::string UniqueLabel = "##InspectorObjectValue" + Utils.HexToString(Address + CurOffset) + MemberObject.Name;
            GUIUtils.ValueRender(UniqueLabel, Address, Address + CurOffset, MemberObject);
            ImGui::SameLine();
            if (ImGui::Button(("Add##AddAPIObject_" + MemberObject.Name).c_str()))
                EventHandler::AddAPIObject(InspectorState.ObjectContentOpenEvent.Address, Address, CurOffset, MemberObject.Bit, MemberObject.Address, DepthPath);
        }

        //��s Nav Line
        GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Update, NavLineIndentSize);

        // �̫�p�G�O Tree Node �h�ۦ��~��B�z�i�}������
        if (GUIUtils.IsExpandType(MemberObject.Type)) {
            // Get State
            GUIUtils.GetDisplayNextState(MemberObject, Address, CurOffset, Indentation, EditorEnable);

            if (Open)
            {
                // ���s�b�h������
                if (InspectorConf.InspectorObjectContentMap.find(TargetObjectName) == InspectorConf.InspectorObjectContentMap.end())
                    EventHandler::UpdateObjectContentPage(MemberObject.ObjectAddress, MemberObject.Type, TargetObjectName, EditorEnable, false, DisplayNextState.NextAddress, DisplayNextState.Size);
                // �w�s�b�h���Indentation
                else {
                    ImGui::SetCursorPosX(InspectorConf.InspectorOffsetIndentation + Indentation + InspectorConf.InspectorTreeObjectNavLineOffset);
                    GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Start);
                    if (GUIUtils.IsExpandType(MemberObject.Type)) DepthPath.push_back(std::make_pair(Address + CurOffset, MemberObject.Address));
                    DisplayInspectorObjectEntry(InspectorConf.InspectorObjectContentMap[TargetObjectName], DepthPath, EditorEnable, DisplayNextState.ShowHeader, DisplayNextState.NextAddress, DisplayNextState.NextOffest, DisplayNextState.NextIndentation);
                    if (GUIUtils.IsExpandType(MemberObject.Type)) DepthPath.pop_back();
                    GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Draw);
                }
                ImGui::TreePop();
            }
        }
    }
}

void Inspector::DisplayInspectorObjectEntry(std::vector<DumperObject>& InspectorObjectSuperList, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath, bool EditorEnable, bool ShowHeader, DWORD_PTR Address, size_t Offest, float Indentation)
{
    for (int i = 0; i < InspectorObjectSuperList.size(); i++) {
        if (ShowHeader) {
            // Name
            ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(0, Indentation, Indentation, true, true));
            ImGui::CopyBtn(InspectorObjectSuperList[i].Name + InspectorObjectSuperList[i].Type + "_NameCopy", InspectorObjectSuperList[i].Name, -1, Style::TreeNodePadding);
            ImGui::SameLine(0, 0);
            int NavLineIndentSize = ImGui::GetCursorPos().x;
            Style::TreeFrameStyleSwitch(true);
            Style::TreeNodeHeaderStyleSwitch(true);
            int Open = ImGui::TreeNodeEx((InspectorObjectSuperList[i].Name + "##" + Utils.HexToString(Address)).c_str(), MainConsoleConf.TreeNodeExFlag);
            Style::TreeNodeHeaderStyleSwitch(false);
            Style::TreeFrameStyleSwitch(false);
            int IndentAccum = Indentation + GUIUtils.GetStringWidth(InspectorObjectSuperList[i].Name);// + InspectorConf.AdditionIndentWidth;

            // Object Address
            ImGui::SameLine();
            ImGui::CopyBtn(InspectorObjectSuperList[i].Name, Utils.HexToString(InspectorObjectSuperList[i].ObjectAddress), GUIUtils.CursorPosCalcu(InspectorConf.InspectorObjectAddressIndentation, Indentation, IndentAccum, true), Style::TreeNodePadding);
            ImGui::SameLine(0, 0);
            ImGui::TextColored(Color::AddressColor, Utils.HexToString(InspectorObjectSuperList[i].ObjectAddress).c_str());

            if (EditorEnable)
            {
                //Address
                ImGui::SameLine();
                ImGui::CopyBtn(InspectorObjectSuperList[i].Name + InspectorObjectSuperList[i].Type + "_EditorEnable", Utils.HexToString(Address), GUIUtils.CursorPosCalcu(InspectorConf.InspectorAddressIndentation, Indentation, IndentAccum), Style::TreeNodePadding);
                ImGui::SameLine(0, 0);
                ImGui::TextColored(Color::AddressColor, Utils.HexToString(Address).c_str());
            }

            if (Indentation != 0) // ���O�Ĥ@�h�h�n�e�u�A��s NavLine ����m [�`�N!!�n�b�i�}���e����s]
                GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Update, NavLineIndentSize);

            if (Open) {
                if (Indentation == 0)   // 0 ��ܬO�Ĥ@�h�A������e TreeNode ���Y�� Indentation �M Offset �� Indentation ���V�q�t
                    InspectorConf.RelativeDistanctWithOffset = ImGui::GetCursorPos().x - InspectorConf.InspectorOffsetIndentation + InspectorConf.InspectorTreeNavLineOffset;// �[���ơA�ץ��U�j�ץ��N�V�h

                if (Indentation != 0)
                    ImGui::SetCursorPosX(GUIUtils.CursorPosCalcu(0, Indentation, Indentation, true, true) + InspectorConf.InspectorTreeMemberNavLineOffset);
                GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Start);
                DisplayInspectorObject(InspectorObjectSuperList[i], DepthPath, EditorEnable, Address, Offest, Indentation);
                GUIUtils.DrawTreeNavLine(InspectorConfig::TreeNavLine::Draw);
                ImGui::TreePop();
            }
        }
        else {
            DisplayInspectorObject(InspectorObjectSuperList[i], DepthPath, EditorEnable, Address, Offest, Indentation);
        }
    }
}


void Inspector::InspectorList()
{
    if (InspectorListResizeable) ChildWindowFlag |= ImGuiChildFlags_ResizeX;
    else                                       ChildWindowFlag &= ~ImGuiChildFlags_ResizeX;
    ImGui::BeginChild("Inspector", ImVec2(InspectorListWindowWidth, 0), ChildWindowFlag);
    {
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Add Inspector", InspectorConf.InspectorList.size());
        ImGui::PopFont();

        // Address Object Inspector
        ImGui::DummySpace();
        ImGui::InputTextWithHintWithWidth("##AddInspector_Input", "Input Object Address", InspectorConf.InputStr);
        ImGui::SameLine();
        if (ImGui::Button(std::string("Add##AddInspector_Btn").c_str(), ImVec2(0, GUIUtils.GetFrameHeight())))
        {
            // �]�w Event => Entry �b��ݰ���
            if (!InspectorConf.InputStr.empty() and InspectorConf.InputStr.length() <= 16 and Utils.IsHex(InspectorConf.InputStr)) {
                EventHandler::AddInspector(std::stoull(InspectorConf.InputStr, nullptr, 16));
            }
        }

        ImGui::DummySpace();
        ImGui::RadioButton("Derived Mode", &InspectorConf.InspectorListMode, InspectorConfig::InspectorListMode::Derived);
        ImGui::DummySpace();
        ImGui::RadioButton("Super Mode", &InspectorConf.InspectorListMode, InspectorConfig::InspectorListMode::Super);
        ImGui::DummySpace();

        // Option 
        if (InspectorConf.InspectorListMode == InspectorConfig::InspectorListMode::Super) {
            ImGui::BeginGroupPanel("Option", ImVec2(ImGui::GetContentRegionAvail().x, 0));
            {
                Style::FramePaddingSwitch(true); ImGui::DummySpace();
                ImGui::Toggle("Editor Mode", &InspectorState.AddInspectorEvent.EditorEnable, ToggleConf);
                ImGui::DummySpace();
                ImGui::Toggle("Is Instance", &InspectorState.AddInspectorEvent.IsInstance, ToggleConf);   // �j��b CreatObectNamePage �ഫ EntryAddress
                Style::FramePaddingSwitch(false); ImGui::DummySpace();
            }
            ImGui::EndGroupPanel();
        }
        else {
            // ���O Super Mode �N�۰����� Editor
            InspectorState.AddInspectorEvent.EditorEnable = false;
        }


        // ���νu
        Style::FramePaddingSwitch(true);
        ImGui::SeparatorText("");
        Style::FramePaddingSwitch(false);

        // Title
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Inspector List");
        ImGui::PopFont();

        // Inspector List
        MyListBox("Inspector List", InspectorConf.InspectorListSelectIndex, InspectorConf.InspectorList, ImVec2(-FLT_MIN, -FLT_MIN - 30), InspectorConf.InspectorListFilterStr, ImGuiSelectableFlags_AllowItemOverlap)
            .SetSelectCallBack([&](int i) {
            InspectorConf.DerivedList.SelectIndex = -1;
            if (InspectorConf.InspectorList[i].Mode == InspectorConfig::InspectorListMode::Derived) {
                InspectorConf.DerivedList.IsOpen = true;
                InspectorConf.DerivedList.CurOpenName = InspectorConf.InspectorList[i].Name;
                InspectorState.ObjectContentOpenEvent.Address = NULL;
            }
            else {
                InspectorConfig::InspectorListObject& InspectorObject = InspectorConf.InspectorList[i];
                InspectorConf.DerivedList.IsOpen = false;
                EventHandler::OpenObjectContentPage(InspectorObject.Address, InspectorObject.Type, InspectorObject.Name, InspectorObject.EditorEnable, InspectorObject.IsInstance);
            }
                })
            .SetPostCallBack([&](int i) {
                    // Delete Btn
                    ImGui::SameLine();
                    int BtnSize = 10;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - BtnSize - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
                    ImGui::PushFont(Font::IconText);
                    Style::TransparentButtonStyleSwitch(true, Color::TextColor);
                    Style::FramePaddingSwitch(true, 2);
                    if (ImGui::Button(std::string(ICON_FA_XMARK "##" + InspectorConf.InspectorList[i].Name + "_ObjectInspectorList_DeleteBtn").c_str(), ImVec2(0, GUIUtils.GetFrameHeight())))
                    {
                        // �R���e���� map ��W
                        GetInspectorObjectContentMapLock.lock();

                        //���� DerivedList
                        InspectorConf.DerivedList.IsOpen = false;  //���� DerivedList Tab

                        if (InspectorConf.InspectorList[i].Mode == InspectorConfig::InspectorListMode::Derived) {
                            // �R�� DerivedList ������ InspectorObjectMap
                            for (auto& Derived : InspectorConf.DerivedList.Map) {
                                InspectorConf.InspectorObjectContentMap.erase(Derived.first);
                            }
                            //�R�� DerivedList
                            InspectorConf.DerivedList.Map[InspectorConf.InspectorList[i].Name].clear();
                        }
                        else {
                            // �R�� ObjectInspectorList ������ InspectorObjectMap
                            InspectorConf.InspectorObjectContentMap.erase(InspectorConf.InspectorList[i].Name);
                        }

                        //�p�G�R�����W�l�O��e���}�������A�h��������
                        if (InspectorConf.InspectorList[i].Name.find(InspectorState.ObjectContentOpenEvent.Name) != std::string::npos)
                            InspectorState.ObjectContentOpenEvent.State = ProcessState::Idle;
                        // �q ObjectInspectorList ���R��
                        InspectorConf.InspectorList.erase(InspectorConf.InspectorList.begin() + i);

                        GetInspectorObjectContentMapLock.unlock();
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    Style::FramePaddingSwitch(false);
                    Style::TransparentButtonStyleSwitch(false);
                    ImGui::PopFont();
                })
                    .Show();


                // ========================= Search Bar =========================
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputTextWithHintWithWidth("##PackageDataInput_Result", "Search Object", InspectorConf.InspectorListFilterStr);
                ImGui::PopItemWidth();
    }
    ImGui::EndChild();
}

void Inspector::DerivedList()
{
    if (!InspectorConf.DerivedList.IsOpen) return;

    ImGui::SameLine(0, 0);
    if (InspectorConf.DerivedListResizeable) ChildWindowFlag |= ImGuiChildFlags_ResizeX;
    else                                     ChildWindowFlag &= ~ImGuiChildFlags_ResizeX;
    ImGui::BeginChild("DerivedList", ImVec2(InspectorConf.DerivedList_Width, 0), ChildWindowFlag);
    {
        // Title
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("Derived List: %d", InspectorConf.DerivedList.Map[InspectorConf.DerivedList.CurOpenName].size());
        ImGui::PopFont();

        // Inspector List
        MyListBox("Derived List", InspectorConf.DerivedList.SelectIndex, InspectorConf.DerivedList.Map[InspectorConf.DerivedList.CurOpenName], ImVec2(-FLT_MIN, -FLT_MIN - 30), InspectorConf.DerivedList.FilterStr)
            .SetSelectCallBack([&](int i) {
                InspectorConfig::InspectorListObject& InspectorObject = InspectorConf.InspectorList[i];
                EventHandler::OpenObjectContentPage(
                    InspectorObject.Address,
                    InspectorObject.Type,
                    InspectorObject.Name,
                    InspectorObject.EditorEnable,
                    InspectorObject.IsInstance
                );
            })
            .Show();

                // ========================= Search Bar =========================
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputTextWithHintWithWidth("##DerivedListInput_Result", "Search Object", InspectorConf.DerivedList.FilterStr);
                ImGui::PopItemWidth();
    }
    ImGui::EndChild();
}

void Inspector::ConditionPage(int Idx, InspectorConfig::ConditionObject& Condition)
{
    if (Condition.IsOpen) {
        ImGui::SetNextWindowPos(MainConsoleConf.CurrentWindowPosition + ((MainConsoleConf.CurentWindowSize - ImVec2(500, 100)) / 2), ImGuiCond_FirstUseEver);
        ImGui::Begin(("[Condition " + std::to_string(Idx + 1) + "] " + Condition.DisplayType + " " + Condition.Value + "###ObjectInstanceSearchConditionList_" + std::to_string(Idx)).c_str(), &Condition.IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
        {
            // Type�BValue
            ImGui::DummySpace();
            MyComboList("MyTest", Condition.TypeListSelectIndex, InspectorConf.TypeList, ImGuiComboFlags_WidthFitPreview)
                .SetCallBack([&](int i) {
                Condition.DisplayType = InspectorConf.TypeList[i];
                Condition.Type = (ValueType)i;
                    })
                .Show();
                    ImGui::SameLine(0, 4);
                    ImGui::InputTextWithHintWithWidth("##ObjectInstanceSearchCondition_Value", " Value ", Condition.Value, 400.f);  // �o�� 500 �M�w window width


                    // Pointer List
                    if (Condition.PointerEnable)
                    {
                        for (int i = (int)Condition.Pointer.size() - 1; i >= 0; i--) {
                            ImGui::PushID(i);
                            ImGui::PushFont(Font::IconText);

                            if (ImGui::Button(ICON_FA_ANGLE_LEFT, ImVec2(30.0f, 0))) {
                                if (!Condition.Pointer[i].empty() && Utils.IsHex(Condition.Pointer[i])) {
                                    size_t Temp = Utils.StringToHex(Condition.Pointer[i]);
                                    Temp = (Temp - 0x4) < 0 ? 0 : (Temp - 0x4);
                                    Condition.Pointer[i] = Utils.HexToString(Temp);
                                }
                            }
                            ImGui::SameLine(0, 4);
                            ImGui::InputTextWithHintWithWidth("##Pointer", " + Pointer Offset ", Condition.Pointer[i], ImGui::GetContentRegionAvail().x - 30.0f - 4.0f);
                            ImGui::SameLine(0, 4);
                            if (ImGui::Button(ICON_FA_ANGLE_RIGHT, ImVec2(30.0f, 0))) {
                                if (!Condition.Pointer[i].empty() && Utils.IsHex(Condition.Pointer[i])) {
                                    size_t Temp = Utils.StringToHex(Condition.Pointer[i]);
                                    Temp += 0x4;
                                    Condition.Pointer[i] = Utils.HexToString(Temp);
                                }
                            }

                            ImGui::PopFont();
                            ImGui::PopID();
                        }
                    }

                    // Base Offset
                    ImGui::InputTextWithHintWithWidth("##ObjectInstanceSearchCondition_Offset", " + Offset ", Condition.Offset, ImGui::GetContentRegionAvail().x);

                    // Pointer Option
                    Style::FramePaddingSwitch(true);
                    if (ImGui::Toggle("Pointer", &Condition.PointerEnable, ToggleConf)) {
                        if (!Condition.PointerEnable)
                            Condition.Pointer.clear();
                    }
                    Style::FramePaddingSwitch(false);
                    ImGui::SameLine();
                    if (ImGui::Button("Add", ImVec2(ImGui::GetContentRegionAvail().x / 2 - 4, 0))) {
                        if (Condition.PointerEnable)
                            Condition.Pointer.push_back("");
                    }
                    ImGui::SameLine(0, 4);
                    if (ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        if (Condition.PointerEnable)
                            Condition.Pointer.pop_back();
                    }
        }
        ImGui::End();
    }
}

void Inspector::InstanceSearchList()
{
    ImGui::SameLine(0, 4);
    ImGui::BeginChild("ObjectInstanceSearchList", ImVec2(ObjectInstanceSearchWindowWidth, 0), true);
    {
        ImVec2 ButtonSize = ImVec2(20.f, 100.f);            // X => Width�B Y => Height
        ImVec2 WindowSize = ImGui::GetWindowSize();
        if (WindowSize.x > ButtonSize.x + 10)
        {     //�h�ԥX 10 (�@���w��)
            float AvailableWindowWidth = ImGui::GetContentRegionAvail().x - ButtonSize.x;

            // Title
            ImGui::PushFont(Font::TitleText);
            {
                ImGui::Text(
                    (TimeObjRef.TimerActivate ? Utils.GetTime(true, TimeObjRef.StartTime, TimeObjRef.EndTime, TimeObjRef.CurTime).c_str()
                        : (TimeObjRef.CurTime.time_since_epoch().count() != 0 ? Utils.GetTime(false, TimeObjRef.StartTime, TimeObjRef.EndTime, TimeObjRef.CurTime).c_str()
                            : "00:00.000"))
                );	//Timer �Ұ� => ��ܷ�e�ɶ� or �w�ϥιL Timer => ��̫ܳ�����ɶ�
                ImGui::Text("Search Result Count : %d", InspectorConf.ObjectInstanceSearchMap.size());
            }
            ImGui::PopFont();

            // Search Option
            ImGui::BeginGroupPanel("Option", ImVec2(AvailableWindowWidth, 0));
            {
                float AvailableGroupWidth = ImGui::GetContentRegionAvail().x - ButtonSize.x;
                ImGui::DummySpace();
                ImGui::Text("Mode");
                ImGui::RadioButton("Normal", &InspectorConf.ObjectInstanceSearchMode, InspectorConfig::SearchMode::Normal);
                ImGui::SameLine();
                ImGui::RadioButton("Condition", &InspectorConf.ObjectInstanceSearchMode, InspectorConfig::SearchMode::Condition);

                if (InspectorConf.ObjectInstanceSearchMode) {
                    ImGui::DummySpace();
                    if (ImGui::CollapsingHeaderWithWidth("Condition List", nullptr, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding, AvailableGroupWidth))
                    {
                        for (int i = 0; i < InspectorConf.ConditionSet.size(); i++) {
                            ImGui::PushID(i);
                            ImGui::DummySpace();
                            if (ImGui::Button(("Condition " + std::to_string(i + 1)).c_str(), ImVec2(AvailableGroupWidth - 30.0f - 4.0f, 0))) {
                                InspectorConf.ConditionSet[i].IsOpen = !InspectorConf.ConditionSet[i].IsOpen;
                            }
                            ImGui::SameLine(0, 4);
                            ImGui::PushFont(Font::IconText);
                            if (ImGui::Button(ICON_FA_XMARK, ImVec2(30.0f, 0))) {
                                // �R�������B��s ID
                                InspectorConf.ConditionSet.erase(InspectorConf.ConditionSet.begin() + i);
                                for (int j = i; j < InspectorConf.ConditionSet.size(); j++) {
                                    InspectorConf.ConditionSet[j].ID = j;
                                }
                                ImGui::PopFont();
                                ImGui::PopID();
                                continue;   //�R���N����V Page�A�������L
                            }
                            ImGui::PopFont();
                            ConditionPage(i, InspectorConf.ConditionSet[i]);
                            ImGui::PopID();
                        }
                        ImGui::DummySpace();
                        if (ImGui::Button("Add Condition", ImVec2(AvailableGroupWidth, 0))) {
                            InspectorConf.ConditionSet.push_back(InspectorConfig::ConditionObject());
                            InspectorConf.ConditionSet[(int)InspectorConf.ConditionSet.size() - 1].ID = (int)InspectorConf.ConditionSet.size();
                        }
                    }
                }

                ImGui::DummySpace();
            }
            ImGui::EndGroupPanel();

            // Search List
            GetObjectInstanceSearchLock.lock();
            MyListBox("Instance Object ListBox", InspectorConf.ObjectInstanceSearchMapSelectedIndex, InspectorConf.ObjectInstanceSearchMap, ImVec2(AvailableWindowWidth, -FLT_MIN - 30), "", ImGuiSelectableFlags_AllowOverlap)
                .SetPostCallBack([&](int i, std::pair<const std::string, DWORD_PTR>& Obj) {
                ImGui::SameLine();
                float Indentation = ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - 10 - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x;
                ImGui::CopyBtn(("ObjectInstanceSearch" + std::to_string(i) + Obj.first), Obj.first, Indentation, 3.0f);
                    })
                .Show();
                    GetObjectInstanceSearchLock.unlock();

                    // ========================= Search Bar =========================
                    float SerachBtnWidth = 60;
                    float SpinnerWidth = InspectorState.ObjectInstanceSearchEvent.State == ProcessState::Processing ? 14 + 4 : 0;   //14 ���| + SameLine Space 4
                    ImGui::PushItemWidth(AvailableWindowWidth - SerachBtnWidth - SpinnerWidth);
                    ImGui::InputTextWithHintWithWidth("##ObjectInstanceSearchInput_Result", "Object Instance Search", InspectorConf.ObjectInstanceSearchInputStr);
                    ImGui::SameLine(0, 0);
                    if (ImGui::Button("Search##ObjectInstanceSearch_SearchBtn", ImVec2(SerachBtnWidth, 0))) {
                        //Search Object (AOB and Filter�BValidation)
                        if (EventHandler::ObjectInstanceSearch(std::string(InspectorConf.ObjectInstanceSearchInputStr)))
                            Utils.TimerSwitch(true, &TimeObjRef.TimerActivate, TimeObjRef.StartTime, TimeObjRef.EndTime);
                    }
                    ImGui::SameLine(0, 4);
                    if (InspectorState.ObjectInstanceSearchEvent.State == ProcessState::Processing)
                        ImGui::Spinner("##spinner", 7, 4, ImGui::ColorConvertFloat4ToU32(Color::SpinnerColor));
                    if (InspectorState.ObjectInstanceSearchEvent.State == ProcessState::Completed and TimeObjRef.TimerActivate)
                        Utils.TimerSwitch(false, &TimeObjRef.TimerActivate, TimeObjRef.StartTime, TimeObjRef.EndTime);

                    ImGui::PopItemWidth();
        }

        ImGui::MoveBtn("ObjectInstanceSearch_MoveBtn", WindowSize, &ObjectInstanceSearchWindowWidth, ButtonSize);
    }
    ImGui::EndChild();
}

void Inspector::InspectorObjectContent()
{
    GetObjectContentMapLock.lock();
    ImGui::SameLine();
    ImGui::BeginChild("InspectorObjectContent", ImVec2(0, 0), ImGuiChildFlags_Border);
    {
        // �ھ� ObjectName ���X������ Page
        if (InspectorState.ObjectContentOpenEvent.State == ProcessState::Completed) {
            // �p�G�� Name �T��b Map �� (�� Bug)
            static std::vector<std::pair<DWORD_PTR, size_t>> DepthPath;
            if (InspectorConf.InspectorObjectContentMap.find(InspectorState.ObjectContentOpenEvent.Name) != InspectorConf.InspectorObjectContentMap.end())
                DisplayInspectorObjectEntry(InspectorConf.InspectorObjectContentMap[InspectorState.ObjectContentOpenEvent.Name], DepthPath, InspectorState.ObjectContentOpenEvent.EditorEnable, true, InspectorState.ObjectContentOpenEvent.Address);
        }

        // =========================  Search Bar  =========================
        static MyInputBarBtn::StateStruct State = MyInputBarBtn::StateStruct::Close;
        static float Timer = 1;
        static ImVec2 Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, ImGui::GetScrollY() + 30);
        Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, ImGui::GetScrollY() + 30);
        MyInputBarBtn("InspectorObjectContentSearchBar", "Search Objec", Timer, 1, 220, InspectorConf.InspectorObjectContentSearchStr, InspectorConf.InspectorObjectContentSearchIdx, SearchMatchCnt, SearchFocus, State)
            .SetPos(Pos).SetFont(Font::IconText).SetSpeed(20.f).Show();
    }
    ImGui::EndChild();
    GetObjectContentMapLock.unlock();
}
