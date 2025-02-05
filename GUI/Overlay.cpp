#pragma once
#include "Overlay.h"

#include "Menu.h"
#include "./Config/FontConfig.h"
#include "./Config/FPSConfig.h"
#include "./Config/MainMenuConfig.h"
#include "../State/GlobalState.h"
#include "../State/WindowState.h"
#include "../State/GUIState.h"

#include "DockingManager.h"
//#include "DropManager.h"

//Font
#include "../font/IconsFontAwesome6.h"
#include "../font/IconRegular.h"
#include "../font/TahomaFont.h"

#include "../Background/Unity/MonoManager.h"

LRESULT Overlay::Wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) //windows msg 傳遞給 ImGui
		return true;

	switch (msg)
	{
		case WM_SYSCOMMAND: {
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
			break;
		}

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}

		case WM_SIZE:
		case WM_DPICHANGED:
		default:
			break;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void Overlay::Setup()
{
	WindowMgr.CreateWindowInstance(Wndproc, L"Jason Hong");
	WindowMgr.MakeFrameIntoClientArea();
	if (D3DRenderMgr.MakeDeviceAndSwapChain(WindowMgr.window) and D3DRenderMgr.CreateRenderTarget()) {
		WindowMgr.InitWindow();
		ImguiSettingMgr.Init(WindowMgr.window, D3DRenderMgr.Device, D3DRenderMgr.DeviceContext);
		RenderLoop(WindowMgr.window, D3DRenderMgr.Device, D3DRenderMgr.DeviceContext, D3DRenderMgr.loadedSwapChain);
	}
	//ImguiSettingMgr.Clear(); //不是在 while 中途退出，所以不用清除
	D3DRenderMgr.CleanupDeviceD3D();
	WindowMgr.DestroyWindowInstance();
	return;
}

void Overlay::RenderLoop(HWND window, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain* loadedSwapChain) 
{
	while (Process::ProcState == Process::State::Running) {
		// ==================  System Msg Process  ==================
		SystemMsgProc();

		// ====================================================
		// 全域的字形設定要先做 (如果有要 Reload 才觸發)
		if (FontConf.ReloadFont) ImguiSettingMgr.SetFont(true);

		// ====================================================
		// Handle window resize (如果 Window 可以 Resize 這邊要去除裡相關狀況)
		//if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		//{
		//	if (RenderTargetView) { RenderTargetView->Release(); RenderTargetView = nullptr; }
		//	loadedSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
		//	g_ResizeWidth = g_ResizeHeight = 0;
		//	CreateRenderTarget();
		//}

		// ==================  GUI Render  ==================
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		MonoMgr.TestLoop(window);

		//ImGui::ShowDemoWindow();
		if (menuToggle) {
			Menu::MenuEntry();
			WindowTransparentUpdate(WindowMgr.window);
		}
		else {
			showTopState = true;
			SetWindowLong(window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);	// WS_EX_TOOLWINDOW 會使在鍵盤上按下 alt+tab 時，找不到該視窗的選項
		}

		// ==================  GUI Clear This Frame  ==================
		ImGui::EndFrame();
		ImGui::Render();

		float color[4]{ 0.f,0.f ,0.f ,0.0f };		//這是螢幕的背景清空的顏色，設定為空
		DeviceContext->OMSetRenderTargets(1, &D3DRenderMgr.RenderTargetView, nullptr);					//函數的目的是告訴 GPU 將渲染的內容輸出到特定的渲染目標上。
		DeviceContext->ClearRenderTargetView(D3DRenderMgr.RenderTargetView, color);				//清除
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		DockingMgr.RenderPostProcess(); // Update and Render additional Platform Windows

		loadedSwapChain->Present(1, 0);

		// ==================  Framte Rate Limit Process  ==================
		LimitFPS();		//不再 Sleep 0.001秒，而是根據使用者需求的 FPS 決定
	}
}

void Overlay::LimitFPS() {

	// ==================  Frame Rate Process ==================
	// Initial When User Change Frame Rate
	if (FPSConf.Initial) {
		QueryPerformanceFrequency(&FPSConf.Frequency);
		FPSConf.Initial = false;
	}

	// Calculate TargetFrameRate 
	FPSConf.TargetFrameRate = 1.0 / FPSConf.TargetFPS;

	// Get Current Time
	QueryPerformanceCounter(&FPSConf.CurrentTime);

	// Waiting
	if (FPSConf.PreviousTime.QuadPart != 0) {	// PreviousTime Is Intitial ?
		double ElapsedTime = static_cast<double>(FPSConf.CurrentTime.QuadPart - FPSConf.PreviousTime.QuadPart) / FPSConf.Frequency.QuadPart;

		if (ElapsedTime < FPSConf.TargetFrameRate) {
			int SleepTime = static_cast<int>((FPSConf.TargetFrameRate - ElapsedTime) * 1000 - 1);
			if (SleepTime > 0) {
				Sleep(SleepTime);
			}
		}
	}

	// Get Previous Time
	QueryPerformanceCounter(&FPSConf.PreviousTime);
}

void Overlay::SystemMsgProc()
{
	// 快捷鍵 Insert 開關視窗
	if (GetAsyncKeyState(VK_INSERT) & 1) menuToggle = !menuToggle;

	// 快捷鍵 End 關閉程式
	if (GetAsyncKeyState(VK_END)) MainMenuState.CloseProcess = ProcessState::Start;

	// 透明化
	// if (GetAsyncKeyState(VK_CONTROL & 'S')) { MainMenuConf.WindowTransparent = false; }
	// if (GetAsyncKeyState(VK_CONTROL & VK_SHIFT & 'S')) { MainMenuConf.WindowTransparent = true; }

	// Message Loop，用於處理 Windows 操作系統的消息。
	// fileDropMgr.initialize(WindowMgr.window);
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {			//PeekMessage 用於檢查消息隊列中是否有消息，但不會阻塞程序。
		TranslateMessage(&msg);								//TranslateMessage  用於轉換某些特殊的鍵盤消息，將虛擬鍵消息轉換為字符消息。
		DispatchMessage(&msg);								//DispatchMessage	用於將消息分派給窗口程序的消息處理函數（Window Procedure）

		if (msg.message == WM_QUIT) { MainMenuState.CloseProcess = ProcessState::Start; break; }
		if (Process::ProcState == Process::State::Stop) break;
	}
	// fileDropMgr.uninitialize(window);	// 不需要收動，自動就被刪了，除非訊息接收是用 GetMessage(&msg, nullptr, 0, 0)
}

void Overlay::WindowTransparentUpdate(HWND window)
{
	//是否要開啟當沒有任何 widge 被 hover 時，Window 就可以被滑鼠穿透
	if (DockingMgr.WindowTransparentChecker(WindowState::WindowTransparentWhenNotHook and !WindowState::IsHookAnyWindow and WindowState::IsMousePosValid) or WindowState::WindowTransparent) {
		SetWindowLong(window, GWL_EXSTYLE, DockingMgr.SetWindowAttribute(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED));
		//每次開啟視窗時顯示在最上層 (WindowShowTop 只要一開起就以他為準，關閉時則以 ShowTopState 為準)
		if (WindowState::WindowShowTop or showTopState) { SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED); showTopState = false; }
		else SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);		//把 WS_EX_TOPMOST 屬性去掉  //可以讓其視窗在上面
	}
	else {
		SetWindowLong(window, GWL_EXSTYLE, DockingMgr.SetWindowAttribute(WS_EX_TOPMOST | WS_EX_LAYERED));
		if (WindowState::WindowShowTop or showTopState) { SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED); showTopState = false; }
		else SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);		//把 WS_EX_TOPMOST 屬性去掉  //可以讓其視窗在上面
	}
}