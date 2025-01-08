#pragma once
#include "../../../../imgui/imgui.h"

#include "../../../Config/ColorConfig.h"
#include "../../../Config/FontConfig.h"
#include "../../../Config/TestComponentConfig.h"
#include "../../../MyGuiComponent.h"

namespace ParamterPanelTab
{
	void Render()
	{
		ImGui::PushFont(Font::TitleText);
		ImGui::ShadowText("TESTTEST", Color::White, Color::SoftBlue, TestComponentConf.FloatSilder1);
		ImGui::PopFont();

		ImGui::DummySpace();
		ImGui::DragFloatWithWidth("FloatSilder1", TestComponentConf.FloatSilder1, 300);

		ImGui::DummySpace();
		ImGui::DragFloatWithWidth("FloatSilder2", TestComponentConf.FloatSilder2, 300);

		ImGui::DummySpace();
		ImGui::DragFloatWithWidth("FloatSilder3", TestComponentConf.FloatSilder3, 300);

		ImGui::DummySpace();
		ImGui::DragFloatWithWidth("FloatSilder4 (0.1)", TestComponentConf.FloatSilder4, 300, 0.1f);

		ImGui::DummySpace();
		ImGui::ColorEdit4("GradientColor1##GradientColor1Color", (float*)&Color::GradientColor1);

		ImGui::DummySpace();
		ImGui::ColorEdit4("GradientColor2##GradientColor2Color", (float*)&Color::GradientColor2);

		ImGui::DummySpace();
		ImGui::ColorButton("Black-Color", Color::Black, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("White-Color", Color::White, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("Green-Color", Color::Green, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("Orange-Color", Color::Orange, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("Gray-Color", Color::Gray, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("lightskyblue-Color", Color::LightskyBlue, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("softBlue-Color", Color::SoftBlue, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("lightBlue-Color", Color::LightBlue, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine();
		ImGui::ColorButton("softYellow-Color", Color::SoftYellow, ImGuiColorEditFlags_NoTooltip);

	}
}