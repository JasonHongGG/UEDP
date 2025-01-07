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
	windowClass.style = CS_HREDRAW | CS_VREDRAW;	//�e�B�����ܮɡA���sø�s
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
		0,		//��
		0,		//�W
		Width,	//�e 
		Height,	//��
		0,
		0,
		windowClass.hInstance,
		0
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);			//�]�w�z����

	this->window = window;
}

void OverlayWindowManager::MakeFrameIntoClientArea() {
	RECT clientArea{};
	GetClientRect(window, &clientArea);		//�����f�����e�ϰ�A���]�A���f����ةM���D��C
	RECT windowArea{};
	GetWindowRect(window, &windowArea);		//�]�t���f���W���M�k�U���b�̹��W���y�СC�]�N�O��ӵ��f���d��
	POINT diff{};
	ClientToScreen(window, &diff);		//��^���f�Ȥ�ϰ쥪�W���b�̹��W���y�СA�o�q�`�O���f���e�����W���C		//�p�G���f�O���̵��f�A����Ȥ�ϰ쪺���W���M�̹��W�����f���W���y�Хi��ۦP�C

	const MARGINS margins{
		windowArea.left + (diff.x - windowArea.left),
		windowArea.top + (diff.y - windowArea.top),
		clientArea.right,
		clientArea.bottom
	};

	DwmExtendFrameIntoClientArea(window, &margins);
	// �o�D�n�ت��O�������᪺�e���@���������I���A�f�t�]�m�������z���סA�������I���ݰ_�ӹ��z����
}

void OverlayWindowManager::InitWindow() {
	// Window ���
	int nShowCmd = SW_SHOWNORMAL;  //or SW_SHOWDEFAULT  SW_SHOWNORMAL  SW_HIDE
	ShowWindow(window, nShowCmd);
	UpdateWindow(window);
}

void OverlayWindowManager::DestroyWindowInstance() {
	//���������A���P class		//�����e���� RegisterClassExW �B CreateWindowExW ��Ӱʧ@������
	DestroyWindow(window);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);		//�̫�@�O�o���P�@�}�l�Ыت� windowClass
}