#pragma once
#include "../../../imgui/imgui.h"
#include "../../Config/MainMenuConfig.h"
#include "../../Config/FontConfig.h"
#include "../../Config/FPSConfig.h"
#include "../../Config/NotificationConfig.h"
#include "../../../State/GUIState.h"
#include "../../../State/WindowState.h"
#include "../../../State/EventHandler.h"
#include "../../MyGuiComponent.h"

namespace ToolPage
{
	void FontSizeBlock(ImGuiIO& io);

	void FrameRateBlock(ImGuiIO& io);

	void MouseAndKeyBoardDetector(ImGuiIO& io);

	void WindowBlock();

	void NotificationBlock();

	void Render(MainMenuCurPage PageID)
	{
		if (PageID != MainMenuCurPage::Tool) return;
		ImGui::BeginChild("ToolOption", ImVec2(0, 0), true);
		{
			ImGuiContext& g = *GImGui;
			ImGuiIO& io = g.IO;

			FontSizeBlock(io);
			ImGui::SeparatorText("");

			FrameRateBlock(io);
			ImGui::SeparatorText("");

			MouseAndKeyBoardDetector(io);
			ImGui::SeparatorText("");

			WindowBlock();
			ImGui::SeparatorText("");

			NotificationBlock();
		}
		ImGui::EndChild();
	}
}


void ToolPage::FontSizeBlock(ImGuiIO& io)
{
	//FontSize
	ImGui::PushFont(Font::TitleText);
	ImGui::Text("Font");
	ImGui::PopFont();
	ImGui::DummySpace();
	{
		ImGui::DragFloat("Proportion", &FontConf.FontProportion, 0.01f, 1.0f, 10.f);
		ImGui::SameLine();
		if (ImGui::Button("Reload")) {
			FontConf.ReloadFont = true;
		}
	}
	ImGui::DragFloat("Scale", &io.FontGlobalScale, 0.005f, 1.f, 3.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void ToolPage::FrameRateBlock(ImGuiIO& io)
{
	// Frame Rate
	ImGui::PushFont(Font::TitleText);
	ImGui::Text("Frame Rate");
	ImGui::PopFont();
	ImGui::DummySpace();
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("Object Data Width", &FPSConf.TargetFPS, 10.0f, 144.0f);
		ImGui::PopItemWidth();
	}
}

void ToolPage::MouseAndKeyBoardDetector(ImGuiIO& io)
{
	// Mouse Key Detector
	ImGui::PushFont(Font::TitleText);
	ImGui::Text("Mouse/Key Detector");
	ImGui::PopFont();
	ImGui::DummySpace();
	{
		ImGui::Text("Keys down:");
		ImGui::SameLine();
		ImGui::Text(
			(MainMenuState.MouseClickKey < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d"
			, ImGui::GetKeyName((ImGuiKey)MainMenuState.MouseClickKey), MainMenuState.MouseClickKey
		);
	}

	ImGui::DummySpace();
	{
		ImGui::Text("IsMousePosValid: %s", ImGui::IsMousePosValid() ? "True" : "False");
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse pos: <INVALID>");
	}
}

void ToolPage::WindowBlock()
{
	// Is Foucs In Window
	ImGui::PushFont(Font::TitleText);
	ImGui::Text("Window Foucs");
	ImGui::PopFont();
	ImGui::DummySpace();
	ImGui::Text("Focus On MainMenu: %s", MainMenuState.FocusOnMainMenu ? "true" : "false");
	ImGui::DummySpace();
	ImGui::Text("Focus On SideBar: %s", MainMenuState.FocusOnSideBar ? "true" : "false");
	ImGui::DummySpace();
	ImGui::Text("io.WantCaptureMouse: %s", WindowState::IsHookAnyWindow ? "true" : "false");

	// WindowTransparentWhenNotHook
	ImGui::DummySpace();
	if (ImGui::Button("WindowTransparent")) {
		WindowState::WindowTransparent = !WindowState::WindowTransparent;
	}
	ImGui::SameLine();
	ImGui::Text("WindowTransparent: %s", WindowState::WindowTransparent ? "true" : "false");

	ImGui::DummySpace();
	if (ImGui::Button("WindowTransparentWhenNotHook")) {
		WindowState::WindowTransparentWhenNotHook = !WindowState::WindowTransparentWhenNotHook;
	}
	ImGui::SameLine();
	ImGui::Text("WindowTransparentWhenNotHook: %s", WindowState::WindowTransparentWhenNotHook ? "true" : "false");

	// WindowShowTop
	ImGui::DummySpace();
	if (ImGui::Button("WindowShowTop")) {
		WindowState::WindowShowTop = !WindowState::WindowShowTop;
	}
	ImGui::SameLine();
	ImGui::Text("WindowShowTop: %s", WindowState::WindowShowTop ? "true" : "false");
}

void ToolPage::NotificationBlock()
{
	// Info
	ImGui::PushFont(Font::TitleText);
	ImGui::Text("Info Toast");
	ImGui::PopFont();
	ImGui::DummySpace();
	{
		ImGui::PushFont(Font::IconText);
		if (ImGui::Button(ICON_FA_ARROW_UP_FROM_BRACKET "None")) {       //"Add Object"
			EventHandler.NotifyEvent(NotificationConfig::NotiyType::None, "", "None Test Message", NotificationConf.DismissTimeBar);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ARROW_UP_FROM_BRACKET "Success")) {       //"Add Object"
			EventHandler.NotifyEvent(NotificationConfig::NotiyType::Success, "", "Success Test Message", NotificationConf.DismissTimeBar);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ARROW_UP_FROM_BRACKET "Warning")) {       //"Add Object"
			EventHandler.NotifyEvent(NotificationConfig::NotiyType::Warning, "", "Warning Test Message", NotificationConf.DismissTimeBar);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ARROW_UP_FROM_BRACKET "Error")) {       //"Add Object"
			EventHandler.NotifyEvent(NotificationConfig::NotiyType::Error, "", "Error Test Message", NotificationConf.DismissTimeBar);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ARROW_UP_FROM_BRACKET "Info")) {       //"Add Object"
			EventHandler.NotifyEvent(NotificationConfig::NotiyType::Info, "", "Info Test Message", NotificationConf.DismissTimeBar);
		}

		ImGui::DragFloat("Dismiss#TestBar", &NotificationConf.DismissTimeBar, (float)0.1);
		ImGui::DragFloat("DescendingSpeed#TestBar", &NotificationConf.DescendingSpeed, (float)0.1);
		ImGui::DragFloat("DecaySpeed#TestBar", &NotificationConf.DecaySpeed, (float)0.1);
		ImGui::PopFont();
	}
}