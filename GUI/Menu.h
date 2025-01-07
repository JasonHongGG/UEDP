#pragma once
#include <d3d11.h>
#include <vector>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

#include "../GUI/Overlay/D3DRender.h"

namespace Menu
{
	inline bool InitialFlag = true;
	void Initial();
	void GetLogo();
	void SetStyle();

	void MenuEntry();
};

