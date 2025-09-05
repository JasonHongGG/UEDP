#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "Operator.h"

// ShadowText / ShadowRectComponent
namespace ImGui {
    inline void ShadowText(const char* label, ImVec4 text_col, ImVec4 shadow_col, float shadow_thickness, const ImVec2& pos_text = ImVec2(-1, -1))
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        if (pos_text.x >= 0 and pos_text.y >= 0)
            ImGui::SetCursorPos(pos_text);

        const ImVec2& size_arg = ImVec2(0, 0);
        ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x, label_size.y);
        ImVec2 pos = window->DC.CursorPos;

        const ImRect bb(pos, pos + size);
        ImGui::ItemSize(size, style.FramePadding.y);

        ImGui::GetWindowDrawList()->AddShadowRect(bb.Min + ImVec2(3, (label_size.y / 2) + 1), bb.Max - ImVec2(3, (label_size.y / 2) - 1), ImGui::GetColorU32(shadow_col), shadow_thickness, ImVec2(0, 0), ImDrawFlags_ShadowCutOutShapeBackground, 100.f);

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(text_col));
        ImGui::RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size);
        ImGui::PopStyleColor();

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    }

    inline void ShadowRectComponent(bool isFilled, ImVec4 color, float thickness, ImVec2 offset, float rounding, ImVec2 Size)
    {
        /*
        Ex:
        ImGui::ShadowRectComponent(
            ShadowPara.shadow_filled,
            ShadowPara.shadow_color,
            ShadowPara.shadow_thickness,
            ShadowPara.shadow_offset,
            ShadowPara.shape_rounding
        );
        */
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 r1(p.x, p.y);
        ImVec2 r2(p.x + Size.x, p.y + Size.y);
        ImDrawFlags draw_flags = isFilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddShadowRect(r1, r2, ImGui::GetColorU32(color), thickness, offset, draw_flags, rounding);
    }
}