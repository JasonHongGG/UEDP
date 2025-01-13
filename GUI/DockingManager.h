#pragma once

#include <windows.h>
#include <iostream>
#include "../imgui/imgui.h"

class DockingManager
{
public:
    bool DockingEnable = false;
    ImGuiWindowFlags_ Disable = ImGuiWindowFlags_NoDocking;
    ImGuiIO* io = nullptr;

    bool WindowTransparentChecker(bool Checker) {
        if (DockingEnable) return false;
        return Checker;
    }

    int SetWindowAttribute(int Attr) {
        if (!DockingEnable) return Attr;
        Attr = Attr | WS_EX_TOOLWINDOW;
        return Attr;
    }

    void Setting() {
        if (!DockingEnable) return;
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    }

    void RenderPostProcess() {
        if (!DockingEnable) return;
        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    ImGuiWindowFlags GetDockingDisableFlag() {
        if (!DockingEnable) return 0;
        return Disable;
    }
private:
};

inline DockingManager DockingMgr = DockingManager();