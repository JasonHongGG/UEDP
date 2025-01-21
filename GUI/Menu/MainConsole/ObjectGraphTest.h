#pragma once
#include <vector>
#include "../../MyGuiComponent/Operator.h"
#include "../../../imgui/imgui.h"
#include "../../Interface/ObjectGraphInterface.h"

inline int ID_Cnt = 0;
inline std::vector<Node*> MainNodes;
inline std::vector<Node*> AllNodes;

namespace ObjectGraph
{
	inline bool initialFlag = false;

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
			n->Name = "Node" + std::to_string(ID_Cnt - 1) + "_" + std::to_string(level);
			n->Pos = GetRandomPosInCircle(root.Pos, root.Radius);
			n->Radius = radius;
			root.To.push_back(n);
			n->From.push_back(&root);
			//if(level > 1)
			GenerateTree(*n, level + 1, radius * 0.6);
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

	inline void Test(std::function<void(std::vector<Node*>& MainNodes, std::vector<Node*>& AllNodes)> DrawNode, std::function<void(std::vector<Node*>& AllNodes)> SegmentIntersectionChecker)
	{
		if (!initialFlag)
			Initial();

		DrawNode(MainNodes, AllNodes);
		SegmentIntersectionChecker(AllNodes);
	}
}