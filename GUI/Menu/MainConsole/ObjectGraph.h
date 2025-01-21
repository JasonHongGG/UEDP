#pragma once
#include <cmath>
#include "../../MyGuiComponent/Operator.h"
#include "../../MyGuiComponent.h"
#include "../../../imgui/imgui.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../../State/EventHandler.h"
#include "../../../Background/Storage/StorageManager.h"	//TODO 要刪除
#include "../../Interface/ObjectGraphInterface.h"
#include "ObjectGraphTest.h"


namespace ObjectGraph
{
	inline bool ShowOverlappingRange = false;
	inline bool ShowExpectDistance = false;
	inline float AvoidOverlappingSpaceRatios = 1.f;
	inline float ExpectDistance = 250.f;
	inline float RepulsiveForce = 1.f;
	inline int GetMostUperImportanceObjectVectorNumber = 10;

	inline bool IsIntersect(ImVec2 A, ImVec2 B, ImVec2 C, ImVec2 D) {
		auto Cross = [](ImVec2 O, ImVec2 P, ImVec2 Q) {
			return (P.x - O.x) * (Q.y - O.y) - (P.y - O.y) * (Q.x - O.x);
			};
		return (Cross(A, B, C) * Cross(A, B, D) < 0) &&
			(Cross(C, D, A) * Cross(C, D, B) < 0);
	}
	inline void SegmentIntersectionChecker(std::vector<Node*>& AllNodes)
	{
		for (Node* n1 : AllNodes) {
			for (Node* x1 : AllNodes) {
				for (Node* n2 : n1->To) {
					for (Node* x2 : x1->To) {
						if (IsIntersect(n1->Pos, n2->Pos, x1->Pos, x2->Pos)) {
							ImVec2 temp;
							temp = n2->Pos;
							n2->Pos = x2->Pos;
							x2->Pos = temp;
						}
					}
				}

			}
		}
	}

	inline void PositionCorrection(Node* n);

	inline void CircleDragHandler(ImVec2 pos, Node* n);

	inline void DrawChildNode(std::vector<Node*>& AllNodes, ImVec2 parentPos, Node* parent);
	inline void DrawNode(std::vector<Node*>& MainNodes, std::vector<Node*>& AllNodes);

	inline void Render(DumperConsoleCurPage& PageID)
	{
		if (PageID != DumperConsoleCurPage::ObjectGraph) return;

		// Object
		if (ImGui::Button("Reset")) {
			ObjectGraphConf.MostUperObjectVector.clear();
			ObjectGraphConf.AllNodes.clear();
			ObjectGraphConf.MainNodes.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Get Most Importance Object"))
			EventHandler::GetMostUperImportanceObjectVector(GetMostUperImportanceObjectVectorNumber);
		ImGui::SameLine();
		std::string BarStr = std::to_string(StorageMgr.ObjectGraphUperProgressBarValue.Get()) + "/" + std::to_string(StorageMgr.ObjectGraphUperProgressBarTotalValue.Get());
		float BarValue = static_cast<float>(StorageMgr.ObjectGraphUperProgressBarValue.Get()) / static_cast<float>(StorageMgr.ObjectGraphUperProgressBarTotalValue.Get());
		ImGui::BufferingBar("##ObjectGraph_BufferBar", BarValue, ImVec2(400, 30), ImGui::GetColorU32(Color::ProgressBarBGColor), ImGui::GetColorU32(Color::ProgressBarColor), BarStr.c_str());
		
		// Config
		int values[3] = {
			GetMostUperImportanceObjectVectorNumber,
			ObjectGraphConf.KeepUperNodeNumber,
			ObjectGraphConf.TreeLevel
		};
		if (ImGui::SliderInt3("Object Number | Keep Uper | Tree Level", values, 1, 10)) {
			GetMostUperImportanceObjectVectorNumber = static_cast<int>(values[0]);
			ObjectGraphConf.KeepUperNodeNumber = static_cast<int>(values[1]);
			ObjectGraphConf.TreeLevel = static_cast<int>(values[2]);
		}
		ImGui::SliderFloat("Expect Range", &ExpectDistance, 10.f, 500.f);
		ImGui::SameLine();
		ImGui::Checkbox("##ShowExpectDistance", &ShowExpectDistance);
		ImGui::SliderFloat("Avoid Overlapping Space Ratios ", &AvoidOverlappingSpaceRatios, 1.f, 20.f);
		ImGui::SameLine();
		ImGui::Checkbox("##ShowOverlappingRange", &ShowOverlappingRange);
		ImGui::SliderFloat("Repulsive Force", &RepulsiveForce, 1.f, 20.f);

		/*DrawNode(ObjectGraphConf.MainNodes, ObjectGraphConf.AllNodes);
		SegmentIntersectionChecker(ObjectGraphConf.AllNodes);*/
		Test(DrawNode, SegmentIntersectionChecker);
	}
}

inline void ObjectGraph::PositionCorrection(Node* n)
{
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 WindowSz = ImGui::GetWindowSize();
	if (n->Pos.x < 0) n->Pos.x = 0;
	if (n->Pos.x > windowPos.x + WindowSz.x) n->Pos.x = windowPos.x + WindowSz.x;
	if (n->Pos.y < 60) n->Pos.y = 60;
	if (n->Pos.y > windowPos.y + WindowSz.y) n->Pos.y = windowPos.y + WindowSz.y;
}

inline void ObjectGraph::CircleDragHandler(ImVec2 pos, Node* n)
{
	ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
	ImGui::PushID(n->ID);
	ImGui::SetCursorScreenPos({ pos.x - n->Radius , pos.y - n->Radius });
	ImGui::InvisibleButton("circle_drag", { n->Radius * 2, n->Radius * 2 });
	if (ImGui::IsItemActive()) {
		// 計算鼠標的移動距離
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		n->Pos.x += delta.x;
		n->Pos.y += delta.y;
	}
	if (ImGui::IsItemHovered()) {
		pWindowDrawList->AddCircle({ pos.x, pos.y }, n->Radius, ImColor(0.f, 0.f, 1.f, 1.f)); // 改變顏色
	}
	ImGui::PopID();
}

inline void ObjectGraph::DrawChildNode(std::vector<Node*>& AllNodes, ImVec2 parentPos, Node* parent)
{
	ImVec2 pos;
	ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
	ImVec2 windowPos = ImGui::GetWindowPos();
	for (int i = 0; i < parent->To.size(); i++) {
		Node* to = parent->To[i];
		pos = windowPos + to->Pos;
		pWindowDrawList->AddCircle({ pos.x, pos.y }, to->Radius, ImColor(1.f, 1.f, 1.f, 1.f));
		CircleDragHandler(pos, to);
		pWindowDrawList->AddText({ pos.x, pos.y }, ImColor(1.f, 1.f, 1.f, 1.f), to->Name.c_str());
		pWindowDrawList->AddLine(parentPos, pos, ImColor(1.f, 1.f, 1.f, 1.f), 2);
		//ImGui::Text("%s - %d _ %f  %f (%f  %f  %f  %f)", "Node", to->ID, pos.x, pos.y, windowPos.x, to->Pos.x, windowPos.y, to->Pos.y);
		if (ShowOverlappingRange) pWindowDrawList->AddCircle({ pos.x, pos.y }, to->Radius * AvoidOverlappingSpaceRatios, ImColor(0.f, 0.f, 1.f, 1.f));
		if (ShowExpectDistance) pWindowDrawList->AddCircle({ pos.x, pos.y }, ExpectDistance, ImColor(1.f, 0.f, 0.f, 1.f));

		// 逐漸靠近
		float reversePushFlag = false;
		float D = Distance(parentPos - pos);
		if (D > ExpectDistance) {
			reversePushFlag = true;
			to->Pos = to->Pos + (parentPos - pos) / (D / 2);
		}

		// 避免重疊
		for (int j = 0; j < AllNodes.size(); j++) {
			if (to->ID == AllNodes[j]->ID) continue;
			Node* another = AllNodes[j];
			ImVec2 anotherPos;
			anotherPos = windowPos + another->Pos;
			float DWithAnother = Distance(anotherPos - pos);
			if (DWithAnother < (to->Radius + another->Radius) * AvoidOverlappingSpaceRatios) {
				// 反方向推走
				float D1 = Distance(parentPos - pos);
				float D2 = Distance(parentPos - anotherPos);
				// 如果此點相較鄰居(another)離 parent 較遠，則推該點，但如果此點距離 parnet 以不夠進，就先推鄰居
				// pos 比 anotherPos 遠，且已在 ExpectDistance 範圍內
				if (D1 > D2 and !reversePushFlag) {
					to->Pos = to->Pos + (pos - anotherPos) / (DWithAnother / RepulsiveForce); //往 pos 推
					PositionCorrection(to);
				}
				else {
					another->Pos = another->Pos + (anotherPos - pos) / (DWithAnother / RepulsiveForce); //往 anotherPos 推
					PositionCorrection(another);
				}


				// 如果原先在 parent 的範圍內，但被推出去，則扯動 parent
				D1 = Distance(parentPos - (windowPos + to->Pos));
				if (D <= ExpectDistance and D1 > ExpectDistance) {
					parent->Pos = parent->Pos + (to->Pos - parent->Pos) / (D1 / RepulsiveForce);
					PositionCorrection(parent);
				}

			}
		}

		if (to->To.size() > 0) DrawChildNode(AllNodes, pos, to);
	}
}

inline void ObjectGraph::DrawNode(std::vector<Node*>& MainNodes, std::vector<Node*>& AllNodes)
{
	ImVec2 parentPos;
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
	for (Node* n : MainNodes) {
		/*n->Pos = mousePos - windowPos;
		parentPos = mousePos;*/
		parentPos = windowPos + n->Pos;
		pWindowDrawList->AddCircle({ parentPos.x, parentPos.y }, n->Radius, ImColor(1.f, 1.f, 1.f, 1.f));
		CircleDragHandler(parentPos, n);
		pWindowDrawList->AddText({ parentPos.x, parentPos.y }, ImColor(1.f, 1.f, 1.f, 1.f), n->Name.c_str());
		if (ShowOverlappingRange) pWindowDrawList->AddCircle({ parentPos.x, parentPos.y }, n->Radius * AvoidOverlappingSpaceRatios, ImColor(0.f, 0.f, 1.f, 1.f));
		if (ShowExpectDistance) pWindowDrawList->AddCircle({ parentPos.x, parentPos.y }, ExpectDistance, ImColor(1.f, 0.f, 0.f, 1.f));

		// child node 繪製
		DrawChildNode(AllNodes, parentPos, n);
	}
}