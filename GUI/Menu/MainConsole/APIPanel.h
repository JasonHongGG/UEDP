#pragma once
#include "../../MyGuiComponent.h"
#include "../../MyGuiUtils.h"
#include "../../Interface/DumperConsoleInterface.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"
#include "../../Config/StyleConfig.h"
#include "../../../State/EventHandler.h"
#include "../../../Utils/Utils.h"
#include "../../../Utils/WindowFileDialog.h"
#include "../../FileDropManager.h"

namespace APIPanel
{
	inline int SearchMatchCnt = 0;
	inline bool SearchFocus = false;

    void DisplayObjectEntry(DumperItem APIListObject, DumperObject& Object, DWORD_PTR Address, size_t Offset, float Indentation);

	void Render(DumperConsoleCurPage& PageID);
}


void APIPanel::DisplayObjectEntry(DumperItem APIListObject, DumperObject& Object, DWORD_PTR Address, size_t Offset, float Indentation)
{
    for (BasicDumperObject& MemberObject : Object.Member)
    {
        int CurOffset = (int)MemberObject.Offset + (int)Offset; //適用於 struct、array、map 等非指標指向新空間的結構


        // =====================  Display  =====================
        // Offset
        std::string BitStr = MemberObject.Bit != -1 ? ":" + std::to_string(MemberObject.Bit) : "";
        ImGui::SetCursorPosX(Indentation);
        int NavLineIndentSize = ImGui::GetCursorPos().x;
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(Color::OffestColor, ("+" + Utils.HexToString(CurOffset) + BitStr).c_str());

        //Type
        ImGui::SameLine();
        Style::TreeFrameStyleSwitch(true);
        Style::TreeNodeHeaderStyleSwitch(true);
        bool Open = false;
        if (GUIUtils.IsExpandType(MemberObject.Type)) {
            // 可展開的 tree node
            Open = ImGui::TreeNodeEx((MemberObject.Type + "##" + MemberObject.Name + "_").c_str(), MainConsoleConf.TreeNodeExFlag);
        }
        else {
            // leaf node
            ImGui::PushStyleColor(ImGuiCol_Text, Color::TypeColor);
            ImGui::TreeNodeEx((MemberObject.Type + "##" + MemberObject.Name + "_").c_str(), MainConsoleConf.TreeNodeExLeafFlag);
            ImGui::PopStyleColor();
        }
        Style::TreeNodeHeaderStyleSwitch(false);
        Style::TreeFrameStyleSwitch(false);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        // Name
        ImGui::SameLine();
        ImGui::TextColored(Color::NameColor, MemberObject.Name.c_str());
        // Hightlight
        ImGui::HighlightStringChecker(MemberObject.Name, APIConf.SearchStr, Color::HighlightColor, &SearchMatchCnt);
        if (SearchFocus and APIConf.SearchIdx == SearchMatchCnt) {
            float posY = ImGui::GetCursorPosY() + ImGui::GetFontSize() - ImGui::GetScrollY();
            ImGui::SetScrollFromPosY(posY);
            SearchFocus = false;
        }

        //Address
        ImGui::SameLine();
        ImGui::CopyBtn(MemberObject.Name + MemberObject.Type + "_EditorEnable", Utils.HexToString(Address + CurOffset), -1, Style::TreeNodePadding);
        ImGui::SameLine(0, 0);
        ImGui::TextColored(Color::AddressColor, Utils.HexToString(Address + CurOffset).c_str());

        //Value
        ImGui::SameLine();
        std::string UniqueLabel = "##APIPannelObjectValue" + Utils.HexToString(Address + CurOffset) + MemberObject.Name;
        if (!GUIUtils.ValueRender(UniqueLabel, Address, Address + CurOffset, MemberObject))
            ImGui::InvisibleButton("None", ImVec2(1, 1));    // 如果失敗，address 是錯的就用一個 btn 擋住剛剛 sameline 的效果

        // 最後如果是 Tree Node 則自行繼續處理展開的部分
        if (GUIUtils.IsExpandType(MemberObject.Type)) {
            // Get State
            GUIUtils.GetDisplayNextState(MemberObject, Address, CurOffset, Indentation, true);

            if (Open)
            {
                // 展開是自己就跳過
                //if (APIListObject.ObjectMap[MemberObject.Address].Address != MemberObject.Address) {
                ImGui::SetCursorPosX(InspectorConf.InspectorOffsetIndentation + Indentation + InspectorConf.InspectorTreeObjectNavLineOffset);
                DisplayObjectEntry(APIListObject, APIListObject.ObjectMap[MemberObject.Address], DisplayNextState.NextAddress, DisplayNextState.NextOffest, DisplayNextState.NextIndentation);
                //}
                ImGui::TreePop();
            }
        }
    }
}

void APIPanel::Render(DumperConsoleCurPage& PageID)
{
    if (PageID != DumperConsoleCurPage::API) return;

    ImGui::BeginChild("APIPannel", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border);
    {
        ImGui::PushFont(Font::TitleText);
        ImGui::Text("API List");
        ImGui::PopFont();

        //Option
        if (ImGui::Button("Update", ImVec2(100, 0))) {
            std::string filePath = winFileDialog.Open();
            if (!filePath.empty())
            {
                std::cout << "Selected file: " << filePath << std::endl;
            }
            EventHandler::UpdateAPIObject();
        }

        ImGui::SameLine(0, 4);
        if (ImGui::Button("Open", ImVec2(100, 0))) {
            std::string filePath = winFileDialog.Open();
			EventHandler::OpenAPIFile(filePath);
        }

        ImGui::SameLine(0, 4);
        if (ImGui::Button("Save", ImVec2(100, 0))) {
            std::string filePath = winFileDialog.Save();
			EventHandler::SaveAPIFile(filePath);
        }

        // Pannel
        ImGui::DummySpace(ImVec2(0.0f, 12.0f));
        for (int i = 0; i < APIConf.APIItemList.size(); i++) {
            static std::string HeaderName;
            HeaderName = "[" + Utils.HexToString(APIConf.APIItemList[i].ID) + "] " + Utils.HexToString(APIConf.APIItemList[i].Address) + " " + APIConf.APIItemList[i].Name + " " + "( Ref: " + APIConf.APIItemList[i].Type + " )";
            if (ImGui::CollapsingHeaderWithWidth(HeaderName.c_str(), &APIConf.APIItemList[i].IsExist, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding, ImGui::GetContentRegionAvail().x))
            {
                DumperItem APIListObject = APIConf.APIItemList[i];
                DisplayObjectEntry(APIListObject, APIListObject.ObjectMap[APIListObject.Address], APIListObject.Address, 0, 0);
            }

            // 關閉就刪除
            if (!APIConf.APIItemList[i].IsExist) {
                APIConf.APIItemList.erase(APIConf.APIItemList.begin() + i);
            }
        }
        // =========================  Search Bar  =========================
        static MyInputBarBtn::StateStruct State = MyInputBarBtn::StateStruct::Close;
        static float Timer = 1;
        static ImVec2 Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, ImGui::GetScrollY() + 10);
        Pos = ImVec2(ImGui::GetContentRegionAvail().x - 30, ImGui::GetScrollY() + 10);
        MyInputBarBtn("APIObjectListSearchBar", "Search Objec", Timer, 1, 220, APIConf.SearchStr, APIConf.SearchIdx, SearchMatchCnt, SearchFocus, State)
            .SetPos(Pos).SetFont(Font::IconText).SetSpeed(20.f).Show();
    }
    ImGui::EndChild();

    // 放在這表示整個視窗都是可以丟檔案的區域 (自動綁定最後一個元素) 
    FileDropMgr.DragSoruceEvent()
        .SetCallBack([&](std::string CurDragDropFilePath) {
		    EventHandler::OpenAPIFile(CurDragDropFilePath);
        })
        .DragTargetEvent();
}