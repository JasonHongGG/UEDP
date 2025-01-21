#pragma once
#include <cmath>
#include "../../MyGuiComponent/Operator.h"
#include "../../MyGuiComponent.h"
#include "../../../imgui/imgui.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../../State/EventHandler.h"
#include "../../../Background/Storage/StorageManager.h"	//TODO �n�R��
#include "../../Interface/ObjectGraphInterface.h"


inline int MaxConnectCount = 10;
inline int ID_Cnt = 0;
inline std::vector<Node*> MainNodes;
inline std::vector<Node*> AllNodes;

namespace ObjectGraph
{
	inline bool initialFlag = false;
	inline bool ShowOverlappingRange = false;
	inline bool ShowExpectDistance = false;
	inline float AvoidOverlappingSpaceRatios = 1.f;
	inline float ExpectDistance = 250.f;
	inline float RepulsiveForce = 1.f;
	inline int GetMostUperObjectVectorNumber = 10;

	inline float Distance(ImVec2 diff)
	{
		return sqrt(diff.x * diff.x + diff.y * diff.y);
	}

	inline ImVec2 GetRandomPos()
	{
		ImVec2 sz = ImGui::GetWindowSize() / 2;
		ImVec2 addition = sz / 2;
		return { addition.x + float(rand() % int(sz.x)), addition.y + float(rand() % int(sz.y)) };
	}

	inline ImVec2 GetRandomPosInCircle(ImVec2 pos, float radius) {
		float r = float(rand()) / RAND_MAX * radius;
		float theta = float(rand()) / RAND_MAX * 2 * M_PI;

		float x = pos.x + r * cos(theta);
		float y = pos.y + r * sin(theta);

		return { x, y };
	}

    inline void GenerateTree(Node& root, int level, int radius)
    {
		if (level > 3) return;

		for (int i = 0; i < level; i++)
		{
			Node* n = new Node();
			AllNodes.push_back(n);
			n->ID = ID_Cnt++;
			n->Name = "Node" + std::to_string(ID_Cnt-1) + "_" + std::to_string(level);
			n->Pos = GetRandomPosInCircle(root.Pos, root.Radius);
			n->Radius = radius;
			root.To.push_back(n);
			n->From.push_back(&root);
			//if(level > 1)
				GenerateTree(*n, level+1, radius * 0.6);
		}
    }

	inline void Initial()
	{
		initialFlag = true;

		Node* FirstNode = new Node();
		AllNodes.push_back(FirstNode);
		FirstNode->ID = ID_Cnt++;
		FirstNode->Name = "FirstNode";
		FirstNode->Pos = GetRandomPos();
		FirstNode->Radius = 20.f;
		GenerateTree(*FirstNode, 2, 20.f * 0.6);

		MainNodes.push_back(FirstNode);
	}	

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

	inline void PositionCorrection(Node* n)
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 WindowSz = ImGui::GetWindowSize();
		if (n->Pos.x < 0) n->Pos.x = 0;
		if (n->Pos.x > windowPos.x + WindowSz.x) n->Pos.x = windowPos.x + WindowSz.x;
		if (n->Pos.y < 60) n->Pos.y = 60;
		if (n->Pos.y > windowPos.y + WindowSz.y) n->Pos.y = windowPos.y + WindowSz.y;
	}

	inline void CircleDragHandler(ImVec2 pos, Node* n)
	{
		ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
		ImGui::PushID(n->ID);
		ImGui::SetCursorScreenPos({ pos.x - n->Radius , pos.y - n->Radius });
		ImGui::InvisibleButton("circle_drag", { n->Radius * 2, n->Radius * 2 });
		if (ImGui::IsItemActive()) {
			// �p�⹫�Ъ����ʶZ��
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			n->Pos.x += delta.x;
			n->Pos.y += delta.y;
		}
		if (ImGui::IsItemHovered()) {
			pWindowDrawList->AddCircle({ pos.x, pos.y }, n->Radius, ImColor(0.f, 0.f, 1.f, 1.f)); // �����C��
		}
		ImGui::PopID();
	}

	inline void DrawChildNode(std::vector<Node*>& AllNodes, ImVec2 parentPos, Node* parent)
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
			if(ShowOverlappingRange) pWindowDrawList->AddCircle({ pos.x, pos.y }, to->Radius * AvoidOverlappingSpaceRatios, ImColor(0.f, 0.f, 1.f, 1.f));
			if(ShowExpectDistance) pWindowDrawList->AddCircle({ pos.x, pos.y }, ExpectDistance, ImColor(1.f, 0.f, 0.f, 1.f));

			// �v���a��
			float reversePushFlag = false;
			float D = Distance(parentPos - pos);
			if (D > ExpectDistance) {
				reversePushFlag = true;
				to->Pos = to->Pos + (parentPos - pos) / (D / 2);
			}

			// �קK���|
			for (int j = 0; j < AllNodes.size(); j++) {
				if (to->ID == AllNodes[j]->ID) continue;
				Node* another = AllNodes[j];
				ImVec2 anotherPos;
				anotherPos = windowPos + another->Pos;
				float DWithAnother = Distance(anotherPos - pos);
				if (DWithAnother < (to->Radius + another->Radius) * AvoidOverlappingSpaceRatios) {
					// �Ϥ�V����
					float D1 = Distance(parentPos - pos);
					float D2 = Distance(parentPos - anotherPos);
					// �p�G���I�۸��F�~(another)�� parent �����A�h�����I�A���p�G���I�Z�� parnet �H�����i�A�N�����F�~
					// pos �� anotherPos ���A�B�w�b ExpectDistance �d��
					if (D1 > D2 and !reversePushFlag) {
						to->Pos = to->Pos + (pos - anotherPos) / (DWithAnother / RepulsiveForce); //�� pos ��
						PositionCorrection(to);
					}
					else {
						another->Pos = another->Pos + (anotherPos - pos) / (DWithAnother / RepulsiveForce); //�� anotherPos ��
						PositionCorrection(another);
					}
						

					// �p�G����b parent ���d�򤺡A���Q���X�h�A�h��� parent
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

	inline void DrawNode(std::vector<Node*>& MainNodes, std::vector<Node*>& AllNodes)
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
			
			// child node ø�s
			DrawChildNode(AllNodes, parentPos, n);
		}
	}

	inline void Render(DumperConsoleCurPage& PageID)
	{
		if (PageID != DumperConsoleCurPage::ObjectGraph) return;

		std::vector<ObjectData> MostUperObjectVector;
		if (ImGui::Button("Reset")) {
			AllNodes.clear();
			MainNodes.clear();
			ObjectGraphConf.MostUperObjectVector.clear();
			ObjectGraphConf.AllNodes.clear();
			ObjectGraphConf.MainNodes.clear();
			initialFlag = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Get Most Uper Object"))
			EventHandler::GetMostUperObjectVector(GetMostUperObjectVectorNumber);
		const ImU32 col = ImGui::GetColorU32(Color::ProgressBarColor);
		const ImU32 bg = ImGui::GetColorU32(Color::ProgressBarBGColor);
		std::string BarStr = std::to_string(StorageMgr.ObjectGraphUperProgressBarValue.Get()) + "/" + std::to_string(StorageMgr.ObjectGraphUperProgressBarTotalValue.Get());
		float BarValue = static_cast<float>(StorageMgr.ObjectGraphUperProgressBarValue.Get()) / static_cast<float>(StorageMgr.ObjectGraphUperProgressBarTotalValue.Get());
		ImGui::SameLine();
		ImGui::BufferingBar("##ObjectGraph_BufferBar", BarValue, ImVec2(400, 30), bg, col, BarStr.c_str());
		ImGui::SameLine();
		ImVec2 mousePos = ImGui::GetMousePos();
		ImGui::Text("Mouse Position : %f %f", mousePos.x, mousePos.y);
		
		ImGui::SetNextItemWidth(200);
		ImGui::SliderInt("Number", &GetMostUperObjectVectorNumber, 1, 10);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::SliderInt("Keep", &ObjectGraphConf.KeepUperNodeNumber, 1, 10);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::SliderInt("Tree Level", &ObjectGraphConf.TreeLevel, 1.f, 10.f);
		ImGui::SliderFloat("Expect Range", &ExpectDistance, 10.f, 500.f);
		ImGui::SameLine();
		ImGui::Checkbox("##ShowOverlappingRange", &ShowOverlappingRange);
		ImGui::SliderFloat("Avoid Overlapping Space Ratios ", &AvoidOverlappingSpaceRatios, 1.f, 20.f);
		ImGui::SameLine();
		ImGui::Checkbox("##ShowExpectDistance", &ShowExpectDistance);
		ImGui::SliderFloat("Repulsive Force", &RepulsiveForce, 1.f, 20.f);

		if (!initialFlag)
			Initial();

		ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
		//ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 WindowSz = ImGui::GetWindowSize();
		ImVec2 windowMaxPos = { windowPos.x + WindowSz.x, windowPos.y + WindowSz.y };
		pWindowDrawList->AddCircle({ mousePos.x, mousePos.y }, 10, ImColor(1.f, 1.f, 1.f, 1.f));

		DrawNode(ObjectGraphConf.MainNodes, ObjectGraphConf.AllNodes);
		SegmentIntersectionChecker(ObjectGraphConf.AllNodes);
	/*	DrawNode(MainNodes, AllNodes);
		SegmentIntersectionChecker(AllNodes);*/
	}
}