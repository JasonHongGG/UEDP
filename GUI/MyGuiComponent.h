#pragma once
#include <cstdlib>
#include <vector>
#include <tuple>
#include <type_traits>
#include <optional>
#include <functional>
#include "d3d11.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../utils/utils.h"
#include "./MyGuiComponent/InputBarBtn.h"
#include "./MyGuiComponent/ListBox.h"
#include "./MyGuiComponent/ComboList.h"
#include "./MyGuiComponent/Operator.h"

#include "Config/FontConfig.h"
#include "Config/ColorConfig.h"
#include "Config/StyleConfig.h"


namespace ImGui {
    inline void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }


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

    inline bool CollapsingHeaderWithWidth(const char* label, bool* isExist, ImGuiTreeNodeFlags flags = 0, float width = 0.0f)
    {
        if (width == 0.0f)
            return ImGui::CollapsingHeader(label, isExist, flags);

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGui::SetNextItemWidth(width);
        float backup_work_max_x = window->WorkRect.Max.x;
        window->WorkRect.Max.x = window->DC.CursorPos.x + ImGui::CalcItemWidth();
        bool ret = ImGui::CollapsingHeader(label, isExist, flags);
        window->WorkRect.Max.x = backup_work_max_x;
        return ret;
    }


    inline void DragFloatWithWidth(const char* label, float &v, float Width = -1, float v_speed = 1.0f)
    {
        if (Width >= 0) ImGui::PushItemWidth(Width);
        ImGui::DragFloat(label, &v, v_speed);
        if (Width >= 0) ImGui::PopItemWidth();
    }


    inline void InputTextWithWidth(const char* label, std::string& text, float Width = 0)
    {
        char TextBuf[128];
        std::string TempInputText = text.length() >= 128 ? text.substr(0, 127) : text;
        strcpy_s(TextBuf, sizeof(TextBuf), TempInputText.c_str());
        if(Width > 0) ImGui::PushItemWidth(Width);
        ImGui::InputText(label, TextBuf, IM_ARRAYSIZE(TextBuf));
        if (Width > 0) ImGui::PopItemWidth();
        text = std::string(TextBuf);    // return back
    }


    inline void InputTextWithHintWithWidth(const char* label, const char* hint, std::string& text, float Width = 0, bool forceSet = false)
    {
        char TextBuf[128];
        strcpy_s(TextBuf, sizeof(TextBuf), text.c_str());
        if (Width > 0 or forceSet) ImGui::PushItemWidth(Width);
        ImGui::InputTextWithHint(label, hint, TextBuf, IM_ARRAYSIZE(TextBuf));
        if (Width > 0 or forceSet) ImGui::PopItemWidth();
        text = std::string(TextBuf);    // return back
    }


    inline void DummySpace(ImVec2 v = ImVec2(0.0f, 4.0f))
    {
        ImGui::Dummy(v);
    }


    inline bool CenterButton(std::string Name, ImVec2 Size)
    {
        float Center = (ImGui::GetWindowWidth() - Size.x) / 2;
        ImGui::SetCursorPosX(Center);
        if (ImGui::Button(Name.c_str(), Size)) {
            return true;
        }
        return false;
    }


    inline void CenterText(std::string Str)
    {
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float StringWidth = ImGui::CalcTextSize(Str.c_str(), NULL, true).x;
        float WindowWidth = ImGui::GetWindowWidth();
        ImGui::SetCursorPosX((WindowWidth - StringWidth) / 2);
        ImGui::Text(Str.c_str());
    }


    inline bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col, const char* overlay) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x;    //前面矩形的寬度就佔總寬的 0.7      //後面要給 圈圈動畫 使用
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;      //圈圈動畫的空間寬度

        //進度條區塊
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col, style.FrameRounding);
        //實際進度條
        if(value > 0.01)       //不要直接初始化，不然數值是零時會有一條直線在畫面上
            window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col, style.FrameRounding);     //輸入的 value 會是一個百分比，進度條的長度，會是矩形程度乘上一個百分比

        const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, value), bb.Max.y);
        ImVec2 overlay_size = CalcTextSize(overlay, NULL);
        if (overlay_size.x > 0.0f)
            RenderTextClipped(ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);

        return true;


        // 以下是圈圈動畫
        /*
        const float t = g.Time;     //開始渲染到現在的時間
        const float r = size.y / 2; //設置動畫圓圈圈的半徑
        const float speed = 1.5f;

        const float a = speed * 0;
        const float b = speed * 0.333f;
        const float c = speed * 0.666f;

        const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;          // => (circleWidth + r) * (t + a - t) / speed       //計算圓點的動畫位置
        const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
        const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
        */
    }


    inline bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;          //當前 render 的位置
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);     //渲染元素的大小

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));       //根據大小創建出一個方形元素
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();  //清除上次的結果

        int num_segments = 30;
        int start = (int)abs(ImSin((float)g.Time * (float)1.8f) * (float)(num_segments - 5));         //開始角度

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;        //開始的圓圈上的角度位置
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;     //結束位置

        const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + (float)ImCos(a + (float)g.Time * (float)8) * radius,              //cos 是 x 的偏移位置 / sin 是 y 軸的偏移位置
                centre.y + (float)ImSin(a + (float)g.Time * (float)8) * radius));
        }

        window->DrawList->PathStroke(color, false, (float)thickness);
        return true;
    }


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

    static ImVector<ImRect> s_GroupPanelLabelStack;
    inline void BeginGroupPanel(const char* name, const ImVec2& size)
    {
        ImGui::BeginGroup();

        auto cursorPos = ImGui::GetCursorScreenPos();
        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetContentRegionAvail().x;
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextUnformatted(name);
        auto labelMin = ImGui::GetItemRectMin();
        auto labelMax = ImGui::GetItemRectMax();
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
        ImGui::BeginGroup();

        ImGui::PopStyleVar(2);

        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->Size.x                   -= frameHeight;

        auto itemWidth = ImGui::CalcItemWidth();
        ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

        s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
    }
    inline void EndGroupPanel()
    {
        ImGui::PopItemWidth();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();

        ImGui::EndGroup();

        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

        ImGui::EndGroup();

        auto itemMin = ImGui::GetItemRectMin();
        auto itemMax = ImGui::GetItemRectMax();
        auto labelRect = s_GroupPanelLabelStack.back();
        s_GroupPanelLabelStack.pop_back();

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
        ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
        labelRect.Min.x -= itemSpacing.x;
        labelRect.Max.x += itemSpacing.x;
        for (int i = 0; i < 4; ++i)
        {
            switch (i)
            {
                // left half-plane
                case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
                // right half-plane
                case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
                // top
                case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
                // bottom
                case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
            }

            ImGui::GetWindowDrawList()->AddRect(
                frameRect.Min, frameRect.Max,
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
                halfFrame.x);

            ImGui::PopClipRect();
        }

        ImGui::PopStyleVar(2);


        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->Size.x                   += frameHeight;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::EndGroup();
    }

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
            float d; //distance 做內插
            if(mode == GradientMode::Horizontal)
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


    inline void GradientComponent(bool activate = false, ImVec4 color1 = ImVec4(0,0,0,0), ImVec4 color2 = ImVec4(0, 0, 0, 0), GradientMode mode = GradientMode::Diagonal)
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

    inline void GradientRect(ImVec2 pos, ImVec2 size, GradientMode mode = GradientMode::Horizontal, ImVec4 color1 = ImVec4(0, 0, 0, 0), ImVec4 color2 = ImVec4(0, 0, 0, 0) )
    {
        static ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
        ImU32 c1 = ImGui::ColorConvertFloat4ToU32(color1);
        ImU32 c2 = ImGui::ColorConvertFloat4ToU32(color2);
        if (mode == GradientMode::Horizontal) 
            pWindowDrawList->AddRectFilledMultiColor(pos,pos + size, c1, c2, c2, c1);
        else if (mode == GradientMode::Vertical)
            pWindowDrawList->AddRectFilledMultiColor(pos, pos + size, c1, c1, c2, c2);
    }


    // Shadow
    inline void ShadowText(const char* label, ImVec4 text_col, ImVec4 shadow_col, float shadow_thickness, const ImVec2& pos_text = ImVec2(-1, -1))
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        if(pos_text.x >= 0 and pos_text.y >= 0)
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

namespace ImGui {
    inline bool TabButtonEx(ImFont* IconFont, const char* ico, const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
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

    //ImGui::TabButton(Menu::IconText, ICON_FA_POWER_OFF, "TESTBtn", ImVec2(ImGui::GetContentRegionAvail().x, 50));
    inline bool TabButton(ImFont* IconFont, const char* ico, const char* label, const ImVec2& size_arg)
    {
        return TabButtonEx(IconFont, ico, label, size_arg, ImGuiButtonFlags_None);
    }
}





template <typename T = float>
class ImTween
{
public:
    enum TweenType
    {
        PingPong
    };

private:
    TweenType type = TweenType::PingPong;
    float speed = 0.5f;
    std::optional<std::function<void()>> onComplete;
    std::function<bool()> condition;

    std::vector<std::tuple<T, T, T*>>&& tweens = {};

    ImTween(std::vector<std::tuple<T, T, T*>>&& tweens)
    {
        this->tweens = std::move(tweens);
    }

public:
    /*
     * accepts unlimited number of values in form of std::tuple<T, T, T*> { min, max, value }
     */
    template <typename T = float, typename... Values>
    constexpr static auto Tween(Values... values) -> ImTween        //constexpr 在編譯出就將所有 function 內的運算計算好 (function 內的所有數值都是預先計算好的)
    {
        static_assert(sizeof...(values) > 0, "No values to animate");
        static_assert((std::is_same_v<std::tuple<T, T, T*>, Values> && ...), "Values must be of type std::tuple<T, T, T*>");

        std::vector<std::tuple<T, T, T*>> tweens;

        (tweens.push_back(values), ...);

        return ImTween<T>(std::move(tweens));
    }

    auto OfType(TweenType&& type) -> ImTween&
    {
        this->type = type;

        return *this;
    }

    auto Speed(float&& speed) -> ImTween&
    {
        this->speed = speed;

        return *this;
    }

    auto When(std::function<bool()>&& condition) -> ImTween&
    {
        this->condition = condition;

        return *this;
    }

    auto OnComplete(std::function<void()>&& onComplete) -> ImTween&
    {
        this->onComplete = onComplete;

        return *this;
    }

    auto Tick()
    {
        for (auto&& tween : tweens)
        {
            auto&& min = std::get<0>(tween);
            auto&& max = std::get<1>(tween);
            auto&& value = *std::get<2>(tween);

            if (this->condition() && value < max)
            {
                value += speed;

                if (value > max)
                    value = max;
            }
            else if ((this->type == TweenType::PingPong) && !this->condition() && value > min)
            {
                value -= speed;

                if (value < min)
                    value = min;
            }

            if (this->onComplete.has_value() && value == max)
                this->onComplete.value()();
        }
    }
};









