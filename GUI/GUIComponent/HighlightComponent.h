#pragma once
#include <string>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "Operator.h"
#include "../../utils/utils.h"

// HighlightStringChecker
namespace ImGui {
    inline void HighlightWithCondition(bool Condition, ImVec4 Color, int* MatchCnt = nullptr) {
        if (Condition) {
            if (MatchCnt) (*MatchCnt) += 1;
            ImVec2 rectMin = ImGui::GetItemRectMin();
            ImVec2 rectMax = ImGui::GetItemRectMax();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImU32 highlightColor = ImGui::ColorConvertFloat4ToU32(Color);
            drawList->AddRectFilled(rectMin, rectMax, highlightColor);
        }
    }

    inline void HighlightStringChecker(std::string& Str1, std::string& Str2, ImVec4 Color, int* MatchCnt = nullptr) {
        std::string MemberName = Utils.rStringToLower(Str1);
        std::string SearchrStr = Utils.rStringToLower(Str2);
        ImGui::HighlightWithCondition(!SearchrStr.empty() and SearchrStr.length() > 1 and MemberName.find(SearchrStr) != std::string::npos, Color, MatchCnt);
    }
}