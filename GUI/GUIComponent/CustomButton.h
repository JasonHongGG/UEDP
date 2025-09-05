#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

#include "../Config/FontConfig.h"
#include "../Config/StyleConfig.h"
#include "../../utils/utils.h"
#include "Operator.h"

// MoveBtn / CopyBtn / TabButton
namespace ImGui {
    inline bool MoveButton(const char* icon, const char* label, ImVec2 buttonSize, float* newCalculatedPos) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImVec2 pos = ImVec2(window->DC.CursorPos.x + 5, window->DC.CursorPos.y);
        const ImRect bb(pos, buttonSize + window->DC.CursorPos);

        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_Button), 4);
        window->DrawList->AddText({ bb.Min.x + buttonSize.x / 2 - CalcTextSize(icon).x / 2, bb.Min.y + buttonSize.y / 2 - CalcTextSize(icon).y }, GetColorU32(ImGuiCol_Text), icon);

        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_None);
        if (hovered) {
            SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (held && newCalculatedPos) {
            *newCalculatedPos += g.IO.MouseDelta.x;
        }

        return held;
    }

    inline void MoveBtn(std::string UniqueID, ImVec2 WindowSize, float* WindowWidthControl, ImVec2 ButtonSize)
    {
        ImGui::PushFont(Font::BigIconText);
        Style::TransparentButtonStyleSwitch(true);
        Style::FramePaddingSwitch(true);
        ImGui::SetCursorPosY(WindowSize.y / 2 - ButtonSize.y / 2);    //50 是 move btn 的一半，如此才會置中
        ImGui::SetCursorPosX(WindowSize.x - ButtonSize.x - 5);
        float CalculatePos = 0;
        if (ImGui::MoveButton(ICON_FA_GRIP_LINES_VERTICAL, "windowmover", ImVec2(ButtonSize.x, ButtonSize.y), &CalculatePos)) {

            if (*(WindowWidthControl) >= ButtonSize.x and *(WindowWidthControl) <= 800) {
                *(WindowWidthControl) += CalculatePos;

                if (*(WindowWidthControl) < ButtonSize.x) *(WindowWidthControl) = ButtonSize.x;
                if (*(WindowWidthControl) > 800)  *(WindowWidthControl) = 800;
            }
        }
        Style::FramePaddingSwitch(false);
        Style::TransparentButtonStyleSwitch(false);
        ImGui::PopFont();
    }

    inline void CopyBtn(std::string UniqueID, std::string CopyContent, float Indentation = -1, float FrameHeight = 0, bool ShowIcon = true, bool ShowCopyContent = false, ImVec4 TextColor = Color::White)
    {
        ImGui::PushFont(Font::IconText);
        Style::TransparentButtonStyleSwitch(true, TextColor);
        Style::FramePaddingSwitch(true, FrameHeight);
        if (Indentation >= 0) ImGui::SetCursorPosX(Indentation);
        if (ImGui::Button(std::string((ShowIcon ? ICON_FA_COPY : "") + (ShowCopyContent ? CopyContent : "") + "##_" + UniqueID).c_str()))
            Utils.CopyToClipBoard(CopyContent);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        Style::FramePaddingSwitch(false);
        Style::TransparentButtonStyleSwitch(false);
        ImGui::PopFont();
    }

    inline bool TabButton(ImFont* IconFont, const char* ico, const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags = ImGuiButtonFlags_None)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        ImVec2 pos = window->DC.CursorPos;
        if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
            pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
        ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        const ImRect bb(pos, pos + size);
        ItemSize(size, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
            flags |= ImGuiButtonFlags_Repeat;

        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

        // Render
        const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        RenderNavHighlight(bb, id);

        static std::map<ImGuiID, float> nameanim_animation;
        auto it_nameanim = nameanim_animation.find(id);
        if (it_nameanim == nameanim_animation.end())
        {
            nameanim_animation.insert({ id, 0.f });
            it_nameanim = nameanim_animation.find(id);
        }
        it_nameanim->second = ImLerp(it_nameanim->second, hovered ? 1.f : 0.150f, g.IO.DeltaTime * 5.f);
        it_nameanim->second *= style.Alpha;

        window->DrawList->AddRectFilledMultiColor(bb.Min + ImVec2(0.000f, 0.000f), bb.Max + ImVec2(bb.Min.x - bb.Min.x - 20, bb.Min.y - bb.Min.y), ImColor(it_nameanim->second, 0.200f, 0.200f, 0.100f), ImColor(0.00f, 0.00f, 0.00f, 0.000f), ImColor(0.00f, 0.00f, 0.00f, 0.000f), ImColor(it_nameanim->second, 0.200f, 0.200f, 0.100f));

        static float gradient_line = 0;
        if (IsItemClicked() || held) gradient_line = 0;
        if (gradient_line <= 190) gradient_line += 0.4f / ImGui::GetIO().Framerate * 60.f;
        static float accent_color[4] = { 0.745f, 0.151f, 0.151f, 1.000f };
        window->DrawList->AddRectFilledMultiColor(ImVec2(0.000f + pos.x, 0.000f + pos.y), ImVec2(gradient_line + pos.x, 40 + pos.y), ImColor(accent_color[0], accent_color[1], accent_color[2], 0.100f), ImColor(accent_color[0], accent_color[1], accent_color[2], 0.000f), ImColor(accent_color[0], accent_color[1], accent_color[2], 0.000f), ImColor(accent_color[0], accent_color[1], accent_color[2], 0.100f));


        const int vtx_idx_1 = GetWindowDrawList()->VtxBuffer.Size;
        PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.45f));
        PushFont(IconFont);
        RenderTextClipped(bb.Min + style.FramePadding + ImVec2(35, 0), bb.Max - style.FramePadding + ImVec2(30, 0), ico, NULL, &label_size, style.ButtonTextAlign, &bb);
        PopFont();
        RenderTextClipped(bb.Min + style.FramePadding + ImVec2(65, 0), bb.Max - style.FramePadding + ImVec2(60, 0), label, NULL, &label_size, style.ButtonTextAlign, &bb);
        PopStyleVar(1);
        const int vtx_idx_2 = GetWindowDrawList()->VtxBuffer.Size;
        ImGui::ShadeVertsLinearColorGradientKeepAlpha(GetWindowDrawList(), vtx_idx_1, vtx_idx_2, bb.Min + ImVec2(20, 0), bb.Max + ImVec2(bb.Min.x - bb.Min.x - 20, 20), ImColor(it_nameanim->second * 2, 0.40f, 0.40f, 1.00f), ImColor(0.05f, 0.05f, 0.05f, 1.00f)); // 蜸馯鳺艜

        return pressed;
    }
}