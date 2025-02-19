#pragma once
#include <vector>
#include "../../MyGuiComponent/Operator.h"
#include "../../../imgui/imgui.h"
#include "../../Interface/ObjectGraphInterface.h"
#include "../../Config/MainConsoleConfig.h"


inline int ID_Cnt = 0;

namespace ObjectGraph
{
	inline bool initialFlag = false;
	inline int MaxLevel = 3;
	inline int MaxTree = 1;

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
		if (level > MaxLevel) return;

		for (int i = 0; i < level; i++)
		{
			Node* n = new Node();
			ObjectGraphConf.AllNodes.push_back(n);
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
		FirstNode->ID = ID_Cnt++;
		FirstNode->Name = "FirstNode";
		FirstNode->Pos = GetRandomPos();
		FirstNode->Radius = 20.f;
		ObjectGraphConf.AllNodes.push_back(FirstNode);
		ObjectGraphConf.MainNodes.push_back(FirstNode);
		GenerateTree(*FirstNode, 2, 20.f * 0.6);
		
	}

	inline void Test()
	{
		if (!initialFlag)
			for(int i = 0; i < MaxTree; i++)
				Initial();

		int values[3] = {
			MaxLevel,
			MaxTree,
		};
		if (ImGui::SliderInt3("MaxLevel | MaxTree", values, 1, 10)) {
			MaxLevel = static_cast<int>(values[0]);
			MaxTree = static_cast<int>(values[1]);
		}

	}
}