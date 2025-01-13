#pragma once
#include <string>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

class GUIUtilsClass
{
public:
    bool IsExpandType(std::string Type);

    // ============================================================================================
    //                                  Style 
    // ============================================================================================

    float GetFrameHeight();

    float GetFontHeight();

};
inline GUIUtilsClass GUIUtils = GUIUtilsClass();

bool GUIUtilsClass::IsExpandType(std::string Type)
{
    if (Type.find("StructProperty") != std::string::npos or
        Type.find("ScriptStruct") != std::string::npos or
        Type.find("ClassProperty") != std::string::npos or
        Type.find("ObjectProperty") != std::string::npos or
        Type.find("Class") != std::string::npos or
        Type.find("ArrayProperty") != std::string::npos or
        Type.find("MapProperty") != std::string::npos)
        return true;
    else return false;
}

float GUIUtilsClass::GetFrameHeight()
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float FrameHeight = ImGui::CalcTextSize("Get GUObject", NULL, true).y + style.FramePadding.y * 2.0f;
    return FrameHeight;
}

float GUIUtilsClass::GetFontHeight()
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float FontHeight = ImGui::CalcTextSize("Get GUObject", NULL, true).y;
    return FontHeight;
}