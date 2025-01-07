#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class OverlayWindowManager
{
public:
    HINSTANCE hInstance = nullptr;
    HWND window = nullptr;
    WNDCLASSEXW windowClass{};

    void CreateWindowInstance(WNDPROC wndProc, LPCWSTR windowName);
    void MakeFrameIntoClientArea();
    void InitWindow();
    void DestroyWindowInstance();

private:
};

inline OverlayWindowManager WindowMgr = OverlayWindowManager();