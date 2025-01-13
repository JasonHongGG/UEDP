#pragma once
#include "../../imgui/imgui.h"
#include "../Config/FontConfig.h"
#include "../Config/MainConsoleConfig.h"
#include "../../State/GUIState.h"
#include "MainConsole/Console.h"
#include "MainConsole/PackageViewer.h"

namespace MainConsole
{
    inline float Width = 1400.f;
    inline float Height = 600.f;
    inline DumperConsoleCurPage PageID = DumperConsoleCurPage::Console;

    inline void MenuBar() {
        ImGui::BeginMenuBar();
        if (ImGui::MenuItem("Console")) PageID = DumperConsoleCurPage::Console;
        if (ImGui::MenuItem("Package Viwer")) PageID = DumperConsoleCurPage::PackageViewer;
        if (ImGui::MenuItem("Inspector")) PageID = DumperConsoleCurPage::Inspector;
        if (ImGui::MenuItem("API")) PageID = DumperConsoleCurPage::API;
        if (ImGui::MenuItem("Setting Pannel")) MainConsoleState.OpenSettingPanel = !MainConsoleState.OpenSettingPanel;
        ImGui::EndMenuBar();
    }

	inline void Render() {
        if (!MainConsoleState.OpenDumperConsole) return;

        ImGui::PushFont(Font::NormalText);
        ImGui::SetNextWindowSize(ImVec2(Width, Height), ImGuiCond_FirstUseEver);
        ImGui::Begin("UE Dumper Console", &MainConsoleState.OpenDumperConsole, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);
        {
            MenuBar();
            Console::Render(PageID);
            PackageViewer::Render(PageID);
            //Inspector();
            //PannelSetting();
            //APIPannel();

            //StateUpdate();
        }
        ImGui::End();
        ImGui::PopFont();
	}
}