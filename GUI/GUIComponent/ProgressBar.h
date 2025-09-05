#pragma once
#include <cstdlib>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

// BufferingBar / Spinner
namespace ImGui {
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
        if (value > 0.01)       //不要直接初始化，不然數值是零時會有一條直線在畫面上
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

}