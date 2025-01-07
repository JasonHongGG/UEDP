#include "OverlayWindow.h"
#include "../DockingManager.h"

void OverlayWindowManager::CreateWindowInstance(WNDPROC Wndproc, LPCWSTR windowname) {
	int Width = GetSystemMetrics(SM_CXSCREEN);
	int Height = GetSystemMetrics(SM_CYSCREEN);

	// Instance Handler 
	HINSTANCE hInstance = GetModuleHandle(NULL);;
	this->hInstance = hInstance;

	// Set Window Attribute
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;	//寬、高改變時，重新繪製
	windowClass.lpfnWndProc = Wndproc;
	//windowClass.cbClsExtra = 0;
	//windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	//windowClass.hIcon = NULL;
	//windowClass.hCursor = NULL;
	//windowClass.hbrBackground = NULL;
	//windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowname;

	// Creat Window
	HWND window = nullptr;
	RegisterClassExW(&windowClass);
	window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_APPWINDOW,
		windowClass.lpszClassName,
		windowClass.lpszClassName,
		WS_POPUP,			// WS_OVERLAPPEDWINDOW     WS_POPUP
		0,		//左
		0,		//上
		Width,	//寬 
		Height,	//高
		0,
		0,
		windowClass.hInstance,
		0
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);			//設定透明度

	this->window = window;
}

void OverlayWindowManager::MakeFrameIntoClientArea() {
	RECT clientArea{};
	GetClientRect(window, &clientArea);		//指窗口的內容區域，不包括窗口的邊框和標題欄。
	RECT windowArea{};
	GetWindowRect(window, &windowArea);		//包含窗口左上角和右下角在屏幕上的座標。也就是整個窗口的範圍
	POINT diff{};
	ClientToScreen(window, &diff);		//返回窗口客戶區域左上角在屏幕上的座標，這通常是窗口內容的左上角。		//如果窗口是全屏窗口，那麼客戶區域的左上角和屏幕上的窗口左上角座標可能相同。

	const MARGINS margins{
		windowArea.left + (diff.x - windowArea.left),
		windowArea.top + (diff.y - windowArea.top),
		clientArea.right,
		clientArea.bottom
	};

	DwmExtendFrameIntoClientArea(window, &margins);
	// 這主要目的是讓視窗後的畫面作為視窗的背景，搭配設置視窗的透明度，讓視窗背景看起來像透明的
}

void OverlayWindowManager::InitWindow() {
	// Window 顯示
	int nShowCmd = SW_SHOWNORMAL;  //or SW_SHOWDEFAULT  SW_SHOWNORMAL  SW_HIDE
	ShowWindow(window, nShowCmd);
	UpdateWindow(window);
}

void OverlayWindowManager::DestroyWindowInstance() {
	//關閉視窗，註銷 class		//對應前面的 RegisterClassExW 、 CreateWindowExW 兩個動作的結束
	DestroyWindow(window);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);		//最後一記得註銷一開始創建的 windowClass
}