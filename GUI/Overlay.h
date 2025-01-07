#pragma once
#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

#include "../System/Process.h"
#include "../System/Memory.h"

#include "Overlay/D3DRender.h"
#include "Overlay/OverlayWindow.h"
#include "Overlay/ImguiSetting.h"
#include "Menu.h"



class Overlay {
public:
	bool menuToggle = true;			//是否顯示視窗
	bool showTopState = true;		//用一個簡單的 State 來確保每次顯示 Menu Window 時都會顯示在最上層

	ImguiSetting ImguiSettingMgr;
	Overlay() {
		ImguiSettingMgr = ImguiSetting();
	}
	~Overlay() {}
	
	static LRESULT Wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Setup();
	void RenderLoop(HWND window, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain* loadedSwapChain);

	// Utils
	void LimitFPS();
	void SystemMsgProc();
	void WindowTransparentUpdate(HWND window);
private:
};

inline Overlay OverlayMgr = Overlay();
