#include "D3DRender.h"

bool D3DRenderManager::MakeDeviceAndSwapChain(HWND window) {
	ZeroMemory(&SwapChain, sizeof(SwapChain));
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;					//�N���ۨC������s 60 ���C
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;					//�o�P�W�������l�@�_��ܨC������s 60 ���A�]�� 60 / 1 = 60�C
	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//�o���w�F�C�ӹ������榡�A�o�̬O 32 �쪺 RGBA �榡�A�C�ӳq�D�ϥ� 8 ��C		(256 * 256 * 256 ���C��)
	SwapChain.SampleDesc.Count = 1;										//�o��ܦh���ļ˪��ƶq�A�o�̳]�m�� 1�A��ܤ��i��h���ļˡC
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;			//�o���w�F�洫�쪺�γ~�A�o�̬O�Ω�@����V�ؼп�X�C
	SwapChain.BufferCount = 2;											//�q�`�ϥ����w�ġA�o�˦b�@�ӽw�İϦb��ܮɡA�t�@�ӥi�H�b��x�i���V�C			
	SwapChain.BufferDesc.Width = 0;
	SwapChain.BufferDesc.Height = 0;
	SwapChain.OutputWindow = window;									//�o�O���w��V�����f���y�`�C				
	SwapChain.Windowed = TRUE;											//�o��ܬO�_�H���f�Ҧ��B��CTRUE ��ܵ��f�Ҧ��AFALSE ��ܥ��̼Ҧ��C			
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;					//�o��ܷ�e�{�U�@�Ӽv��ɡA�p��B�z�e�@�ӽw�Ī����e�C		DXGI_SWAP_EFFECT_DISCARD ��ܦb�e�{�U�@�ӽw�ĮɡA���e�@�ӽw�Ī����e�C			
	SwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;			//�o�O�@�ռлx�A�o�� DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ���\�洫��b���̩M���f�Ҧ������i������C	

	// �Ω�Ы� Direct3D 11 �˸m�]Device�^�B�˸m�W�U��]Device Context�^�H�Υ洫��]Swap Chain�^����ơC
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
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);		//����w�ĳЫؤ@��RenderTargetView�A�ϱo��w�ĥi�H�Q�Χ@��V�ؼСA�åB����N��V�����e��ܦb���f�W�C
		backBuffer->Release();	//�i�H�Ҽ{������A�᭱�N�i�������ƨϥΡA���ݭn�A�S�O�Ҽ{��ɻݭn����
		return true;
	}
	return false;
}

void D3DRenderManager::CleanupDeviceD3D()
{
	//�����ϧδ�V�����X��
	//�p�G����s�b�A�h�R���o�Ǫ���
	if (loadedSwapChain) { loadedSwapChain->Release(); loadedSwapChain = nullptr; }
	if (DeviceContext) { DeviceContext->Release(); DeviceContext = nullptr; }
	if (Device) { Device->Release(); Device = nullptr; }
	if (RenderTargetView) { RenderTargetView->Release(); RenderTargetView = nullptr; }
}