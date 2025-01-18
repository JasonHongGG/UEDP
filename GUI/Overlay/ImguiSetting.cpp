#include "ImguiSetting.h"
#include "../DockingManager.h"
#include "../Config/FontConfig.h"
#include "../../Font/IconsFontAwesome6.h"
#include "../../Font/IconRegular.h"
//#include "../../Font/TahomaFont.h"
//#include "../../Font/NotoSans.h"

void ImguiSetting::Init(HWND window, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) {
	// Setup ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	DockingMgr.Setting();

	//關閉紀錄在 Imgui.ini
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	// Setup Default Style  
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(Device, DeviceContext);

	// Setup Font	(在 NewFrame 前做即可)
	SetFont();
};
void ImguiSetting::Clear() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
};

void ImguiSetting::SetFont(bool IsReload)
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// 系統字體
	Font::NormalText = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", FontConf.NormalFontSize * FontConf.FontProportion);

	if (IsReload) {
		FontConf.ReloadFont = false;
		io.Fonts->Build();
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}
	else {
		Font::TitleText = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanab.ttf", FontConf.TitleFontSize * FontConf.FontProportion);
		Font::SubTitleText = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanab.ttf", FontConf.SubTitleFontSize * FontConf.FontProportion);
		Font::HighlightText = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanai.ttf", FontConf.HighlightFontSize * FontConf.FontProportion);
		Font::ChineseText = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", FontConf.ChineseFontSize * FontConf.FontProportion, NULL, io.Fonts->GetGlyphRangesChineseFull());

		// Icon 字體
		FontWrapper(Font::IconText, FontConf.IconFontSize * FontConf.FontProportion, true, "C:\\Windows\\Fonts\\Verdana.ttf");
		FontWrapper(Font::BigIconText, FontConf.BigIconFontSize * FontConf.FontProportion, true, "C:\\Windows\\Fonts\\Verdana.ttf");
		FontWrapper(Font::NotifyText, FontConf.NotifyFontSize * FontConf.FontProportion, true, "C:\\Windows\\Fonts\\msyh.ttc");
		FontWrapper(Font::NotifyContentText, FontConf.NotifyContentFontSize * FontConf.FontProportion, true, "C:\\Windows\\Fonts\\msyh.ttc");
		//FontWrapper(io, baseFontSize, Menu::IconText, NULL, false, (void*)tahoma, sizeof(tahoma));
		//FontWrapper(io, (baseFontSize * 3.f), Menu::LogoIconText, "C:\\Windows\\Fonts\\msyh.ttc");

		// 內嵌字體
		//FontWrapper(Font::TahomaText, 17.f, true, NULL, (void*)tahoma, sizeof(tahoma));
		//FontWrapper(false, io, 17.f, Font::NotoSans, NULL, true, (void*)noto_sans, sizeof(noto_sans));
	}
}

void ImguiSetting::FontWrapper(ImFont*& TarFont, float FontSize, bool IsIconFont, const char* FontPath, void* Font, int FontArrSize)
{
	ImGuiIO& io = ImGui::GetIO();
	float IconFontSize = FontSize * 2.0f / 3.0f;
	ImFontConfig IconFontConfig;
	IconFontConfig.MergeMode = true;
	IconFontConfig.PixelSnapH = true;
	IconFontConfig.GlyphMinAdvanceX = IconFontSize;

	// 主要字體
	if (FontPath) {
		TarFont = io.Fonts->AddFontFromFileTTF(FontPath, (FontSize > 0 ? FontSize : 16.f), NULL, io.Fonts->GetGlyphRangesChineseFull());
	}
	else {
		ImFontConfig FontConfig;
		FontConfig.FontDataOwnedByAtlas = false;
		IconFontConfig.FontDataOwnedByAtlas = false;
		TarFont = io.Fonts->AddFontFromMemoryTTF((void*)Font, FontArrSize, (FontSize > 0 ? FontSize : 16.f), &FontConfig);
	}

	// Icon 字體 (綁定到主要字體上)
	if (IsIconFont) {
		static const ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		//TarFont = io.Fonts->AddFontFromFileTTF("./font/fa-solid-900.ttf", IconFontSize, &IconFontConfig, iconsRanges);		//fa-regular-400.ttf  //fa-solid-900.ttf
		TarFont = io.Fonts->AddFontFromMemoryTTF((void*)iconRegular, sizeof(iconRegular), IconFontSize, &IconFontConfig, iconsRanges);
	}
}