#include "D3DRender.h"

bool D3DRenderManager::MakeDeviceAndSwapChain(HWND window) {
	ZeroMemory(&SwapChain, sizeof(SwapChain));
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;					//意味著每秒頁面更新 60 次。
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;					//這與上面的分子一起表示每秒頁面更新 60 次，因為 60 / 1 = 60。
	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//這指定了每個像素的格式，這裡是 32 位的 RGBA 格式，每個通道使用 8 位。		(256 * 256 * 256 種顏色)
	SwapChain.SampleDesc.Count = 1;										//這表示多重採樣的數量，這裡設置為 1，表示不進行多重採樣。
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;			//這指定了交換鏈的用途，這裡是用於作為渲染目標輸出。
	SwapChain.BufferCount = 2;											//通常使用雙緩衝，這樣在一個緩衝區在顯示時，另一個可以在後台進行渲染。			
	SwapChain.BufferDesc.Width = 0;
	SwapChain.BufferDesc.Height = 0;
	SwapChain.OutputWindow = window;									//這是指定渲染的窗口的句柄。				
	SwapChain.Windowed = TRUE;											//這表示是否以窗口模式運行。TRUE 表示窗口模式，FALSE 表示全屏模式。			
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;					//這表示當呈現下一個影格時，如何處理前一個緩衝的內容。		DXGI_SWAP_EFFECT_DISCARD 表示在呈現下一個緩衝時，丟棄前一個緩衝的內容。			
	SwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;			//這是一組標誌，這裡 DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH 允許交換鏈在全屏和窗口模式之間進行切換。	

	// 用於創建 Direct3D 11 裝置（Device）、裝置上下文（Device Context）以及交換鏈（Swap Chain）的函數。
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, FeatureLevels, 3, D3D11_SDK_VERSION, &SwapChain, &loadedSwapChain, &Device, &FeatureLevel, &DeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, FeatureLevels, 3, D3D11_SDK_VERSION, &SwapChain, &loadedSwapChain, &Device, &FeatureLevel, &DeviceContext);
	if (res != S_OK)
		return false;
	return true;
}

bool D3DRenderManager::CreateRenderTarget()
{
	loadedSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (backBuffer) {
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);		//為後緩衝創建一個RenderTargetView，使得後緩衝可以被用作渲染目標，並且能夠將渲染的內容顯示在窗口上。
		backBuffer->Release();	//可以考慮不釋放，後面就可直接重複使用，但需要再特別考慮何時需要釋放
		return true;
	}
	return false;
}

void D3DRenderManager::CleanupDeviceD3D()
{
	//關閉圖形渲染相關驅動
	//如果物件存在，則刪除這些物件
	if (loadedSwapChain) { loadedSwapChain->Release(); loadedSwapChain = nullptr; }
	if (DeviceContext) { DeviceContext->Release(); DeviceContext = nullptr; }
	if (Device) { Device->Release(); Device = nullptr; }
	if (RenderTargetView) { RenderTargetView->Release(); RenderTargetView = nullptr; }
}