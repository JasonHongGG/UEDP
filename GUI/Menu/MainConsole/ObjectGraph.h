#pragma once
#pragma once
#include <cmath>
#include <algorithm>
#include "../../MyGuiComponent/Operator.h"
#include "../../MyGuiComponent.h"
#include "../../MyGuiUtils.h"
#include "../../../imgui/imgui.h"
#include "../../Config/MainConsoleConfig.h"
#include "../../../State/EventHandler.h"
#include "../../../Background/Storage/StorageManager.h"	
#include "../../Interface/ObjectGraphInterface.h"
#include "ObjectGraphTest.h"


namespace ObjectGraph {
    inline bool ShowOverlappingRange = false;
    inline bool ShowExpectDistance = false;
    inline Node* FocusNode = nullptr;
    inline Node* DragNode = nullptr;
    inline float ExpectDistance = 150.f;
    inline float RepulsiveForce = 0.1f;
    inline float DampingFactor = 0.9f; // 用於減速模擬
    inline float AttractionForce = 0.6f;
    inline float AvoidOverlappingSpaceRatios = 2.f;
    inline int GetMostUperImportanceObjectVectorNumber = 10;


    inline float Length(ImVec2& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

	inline float Distance(Node* nodeA, Node* nodeB)
	{
		ImVec2 diff = nodeA->Pos - nodeB->Pos;
		return Length(diff);
	}

    inline ImVec2 SplitVector(ImVec2& v, float ratioA, float ratioB) {
        float totalRatio = std::sqrt(ratioA * ratioA + ratioB * ratioB);
        float length = Length(v);

        // 計算比例長度
        float lengthA = (ratioA / totalRatio) * length;
        float lengthB = (ratioB / totalRatio) * length;

        // 計算單位向量
        ImVec2 unitOriginal = { v.x / length, v.y / length };   // 原方向單位向量
        ImVec2 unitPerpendicular = { -unitOriginal.y, unitOriginal.x }; // 垂直方向單位向量

        // 計算分量
        ImVec2 componentA = unitPerpendicular * lengthA; // 垂直方向
        ImVec2 componentB = unitOriginal * lengthB;      // 原方向

        return componentA + componentB;
    }

    inline void PropagateForces(Node* SourceNode, Node* node, ImVec2 force, float attenuation = 0.2f) {
        if (!node || attenuation < 0.1f) return; // 終止傳遞的條件

        // 增加力量到節點
        if(SourceNode != node)
            node->Velocity = node->Velocity + force;

        // 衰減傳遞的力量
        ImVec2 propagatedForce = force * attenuation;

        // 遞迴傳遞給所有連接的節點
        for (Node* connectedNode : node->To) {
            if (SourceNode == connectedNode) continue;
			if (Distance(SourceNode, connectedNode) <= ExpectDistance) continue;
            PropagateForces(node, connectedNode, propagatedForce, attenuation);
        }
        for (Node* connectedNode : node->From) {
            if (SourceNode == connectedNode) continue;
            if (Distance(SourceNode, connectedNode) <= ExpectDistance) continue;
            PropagateForces(node, connectedNode, propagatedForce, attenuation);
        }
    }

    inline void PositionCorrection(Node* node)
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        node->Pos.x = std::clamp(node->Pos.x, windowPos.x, windowPos.x + windowSize.x);
        node->Pos.y = std::clamp(node->Pos.y, windowPos.y, windowPos.y + windowSize.y);
    }

    inline void ApplyAttractionForce(Node* node, Node* centerNode)
    {
        if (!node || !centerNode || node == centerNode || node == DragNode) return;

        ImVec2 delta = centerNode->Pos - node->Pos;
        float diff = sqrt(delta.x * delta.x + delta.y * delta.y);

        // Attraction towards center node
        if (diff > ExpectDistance) {
            ImVec2 attraction = (delta / diff) * AttractionForce;
            node->Velocity = node->Velocity + attraction;
        }
    }

    inline void ApplyRepulsiveForces(Node* node) {
        if (!node) return;

        // Repulsive forces between nodes
        for (Node* other : ObjectGraphConf.AllNodes) {
            if (node == other) continue;
			if (node == DragNode) continue; // Skip if dragging

            ImVec2 diff = node->Pos - other->Pos;
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
            float combinedRadiusOther = (node->Radius + other->Radius) * AvoidOverlappingSpaceRatios;
            if (dist < combinedRadiusOther) {
                ImVec2 repulsion = (diff / dist) * (RepulsiveForce * (combinedRadiusOther - dist));
				repulsion = SplitVector(repulsion, 7.f, 3.f); // Split the repulsion force // 不要回彈而是錯開
                node->Velocity = node->Velocity + repulsion;
            }
        }
    }

    inline void PositionUpdate(Node* node)
    {
        if (!node) return;
        // Apply damping to slow down motion over time
        node->Velocity = node->Velocity * DampingFactor;

        // Update position
        node->Pos = node->Pos + node->Velocity;

        PositionCorrection(node);
    }

    inline void DragHandle(Node* node, ImVec2 pos)
    {
        ImGui::PushID(node->ID);
        ImGui::SetCursorScreenPos(pos - ImVec2(node->Radius, node->Radius));
        ImGui::InvisibleButton("node_drag", ImVec2(node->Radius * 2, node->Radius * 2));
        if (ImGui::IsItemActive()) {
            DragNode = node;
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            node->Pos = node->Pos + delta;

            PropagateForces(node, node, delta);
        }
        ImGui::PopID();
    }

    inline void RenderNode(Node* node, bool isFocus) {
        if (!node) return;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 pos = windowPos + node->Pos;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
		float NameLenght = GUIUtils.GetStringWidth(node->Name);
        drawList->AddCircleFilled(pos, node->Radius, isFocus ? IM_COL32(255, 100, 100, 255) : IM_COL32(255, 255, 255, 255));
        drawList->AddText(pos - ImVec2(NameLenght / 2, -node->Radius), IM_COL32(255, 255, 255, 255), node->Name.c_str());
        if (ShowOverlappingRange) drawList->AddCircle(pos, node->Radius * AvoidOverlappingSpaceRatios, ImColor(0.f, 0.f, 1.f, 1.f));
        if (ShowExpectDistance) drawList->AddCircle(pos, ExpectDistance, ImColor(1.f, 0.f, 0.f, 1.f));

        // Handle dragging
        DragHandle(node, pos);
    }

    inline void RenderGraph() {
        if (!FocusNode && !ObjectGraphConf.AllNodes.empty()) {
            FocusNode = ObjectGraphConf.AllNodes[0]; // Default focus
        }

        // Apply forces and render each node
        for (Node* node : ObjectGraphConf.AllNodes) {
            if (node->From.size() > 0)
                ApplyAttractionForce(node, node->From[0]);
            ApplyRepulsiveForces(node);
            PositionUpdate(node);
            RenderNode(node, node == FocusNode);

            // Draw connections
            for (Node* to : node->To) {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddLine(node->Pos + ImGui::GetWindowPos(), to->Pos + ImGui::GetWindowPos(), IM_COL32(200, 200, 200, 255), 2.0f);
            }
        }
    }


    inline void Render(DumperConsoleCurPage & PageID)
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
        ImGui::SliderFloat("Repulsive Force", &RepulsiveForce, 0.01f, 2.f);
        ImGui::SliderFloat("Attraction Force", &AttractionForce, 0.1f, 10.f);
        
        Test();

        RenderGraph();
    }

}