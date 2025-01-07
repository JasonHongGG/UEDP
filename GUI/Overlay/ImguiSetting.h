#pragma once
#include <Windows.h>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"

class ImguiSetting
{
public:
	void Init(HWND window, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
	void Clear();
	void SetFont(bool IsReload = false);
	void FontWrapper(ImFont*& TarFont, float FontSize, bool IsIconFont = false, const char* FontPath = nullptr, void* Font = nullptr, int FontArrSize = 0);

private:
};