#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

#include "./InputBarBtn.h"
#include "./ListBox.h"
#include "./ComboList.h"
#include "./Operator.h"

#include "./CustomButton.h"
#include "./GradientComponent.h"
#include "./GroupPanel.h"
#include "./HighlightComponent.h"
#include "./ProgressBar.h"
#include "./ResizableComponent.h"
#include "./ShadowComponent.h"
#include "./Tween.h"


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
}
