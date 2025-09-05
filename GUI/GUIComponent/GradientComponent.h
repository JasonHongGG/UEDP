#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "Operator.h"

// GradientComponent/ GradientRect
namespace ImGui {
    // Gradient
    enum class GradientMode {
        Horizontal = 0,
        Vertical,
        Diagonal
    };

    inline void MyShadeVertsLinearColorGradient(GradientMode mode, ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
    {
        ImVec2 gradient_extent = gradient_p1 - gradient_p0;
        float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
        ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
        ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
        const int col0_r = (int)(col0 >> IM_COL32_R_SHIFT) & 0xFF;
        const int col0_g = (int)(col0 >> IM_COL32_G_SHIFT) & 0xFF;
        const int col0_b = (int)(col0 >> IM_COL32_B_SHIFT) & 0xFF;
        const int col_delta_r = ((int)(col1 >> IM_COL32_R_SHIFT) & 0xFF) - col0_r;
        const int col_delta_g = ((int)(col1 >> IM_COL32_G_SHIFT) & 0xFF) - col0_g;
        const int col_delta_b = ((int)(col1 >> IM_COL32_B_SHIFT) & 0xFF) - col0_b;
        for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
        {
            float d; //distance °µ¤º´¡
            if (mode == GradientMode::Horizontal)
                d = ImDot(ImVec2(vert->pos.x - gradient_p0.x, 0), gradient_extent);
            else if (mode == GradientMode::Vertical)
                d = ImDot(ImVec2(0, vert->pos.y - gradient_p0.y), gradient_extent);
            else if (mode == GradientMode::Diagonal)
                d = ImDot(vert->pos - gradient_p0, gradient_extent);

            float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
            int r = (int)(col0_r + col_delta_r * t);
            int g = (int)(col0_g + col_delta_g * t);
            int b = (int)(col0_b + col_delta_b * t);
            vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
        }
    }

    inline void GradientComponent(bool activate = false, ImVec4 color1 = ImVec4(0, 0, 0, 0), ImVec4 color2 = ImVec4(0, 0, 0, 0), GradientMode mode = GradientMode::Diagonal)
    {
        static ImDrawList* pWindowDrawList;
        static int vtx_idx_3;
        static int vtx_idx_4;

        if (activate) {
            pWindowDrawList = ImGui::GetWindowDrawList();
            vtx_idx_3 = pWindowDrawList->VtxBuffer.Size;
        }
        else {
            vtx_idx_4 = pWindowDrawList->VtxBuffer.Size;
            ImVec2 rectMin = ImGui::GetItemRectMin();
            ImVec2 rectMax = ImGui::GetItemRectMax();
            ImGui::MyShadeVertsLinearColorGradient(mode, pWindowDrawList, vtx_idx_3, vtx_idx_4, rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(color1), ImGui::ColorConvertFloat4ToU32(color2));
        }
    }

    inline void GradientRect(ImVec2 pos, ImVec2 size, GradientMode mode = GradientMode::Horizontal, ImVec4 color1 = ImVec4(0, 0, 0, 0), ImVec4 color2 = ImVec4(0, 0, 0, 0))
    {
        static ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
        ImU32 c1 = ImGui::ColorConvertFloat4ToU32(color1);
        ImU32 c2 = ImGui::ColorConvertFloat4ToU32(color2);
        if (mode == GradientMode::Horizontal)
            pWindowDrawList->AddRectFilledMultiColor(pos, pos + size, c1, c2, c2, c1);
        else if (mode == GradientMode::Vertical)
            pWindowDrawList->AddRectFilledMultiColor(pos, pos + size, c1, c1, c2, c2);
    }
}