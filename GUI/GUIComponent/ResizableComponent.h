#pragma once
#include <string>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

// CollapsingHeaderWithWidth
// DragFloatWithWidth
// InputTextWithWidth / InputTextWithHintWithWidth
namespace ImGui {

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


    inline void DragFloatWithWidth(const char* label, float& v, float Width = -1, float v_speed = 1.0f)
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
        if (Width > 0) ImGui::PushItemWidth(Width);
        ImGui::InputText(label, TextBuf, IM_ARRAYSIZE(TextBuf));
        if (Width > 0) ImGui::PopItemWidth();
        text = std::string(TextBuf);    // return back
    }


    inline void InputTextWithHintWithWidth(const char* label, const char* hint, std::string& text, float Width = 0, bool forceSet = false)
    {
        char TextBuf[128];
        strcpy_s(TextBuf, sizeof(TextBuf), text.c_str());
        if (Width != 0 or forceSet) ImGui::PushItemWidth(Width);
        ImGui::InputTextWithHint(label, hint, TextBuf, IM_ARRAYSIZE(TextBuf));
        if (Width != 0 or forceSet) ImGui::PopItemWidth();
        text = std::string(TextBuf);    // return back
    }

}
