#pragma once
#include <d3d11.h>

class D3DRenderManager {
public:
	DXGI_SWAP_CHAIN_DESC SwapChain{};
	D3D_FEATURE_LEVEL FeatureLevels[3]{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};
	ID3D11Device* Device{ 0 };			//這樣子的宣告方式都是空指標，表示我待會會用到這些東西，先宣告
	ID3D11DeviceContext* DeviceContext{ 0 };
	IDXGISwapChain* loadedSwapChain{ 0 };
	ID3D11RenderTargetView* RenderTargetView{ 0 };
	D3D_FEATURE_LEVEL FeatureLevel{};
	ID3D11Texture2D* backBuffer{ 0 };

	bool MakeDeviceAndSwapChain(HWND window);
	bool CreateRenderTarget();
	void CleanupDeviceD3D();
};

inline D3DRenderManager D3DRenderMgr = D3DRenderManager();