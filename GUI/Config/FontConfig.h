#pragma once
#include "../../imgui/imgui.h"

namespace Font
{
	inline ImFont* NormalText;
	inline ImFont* TitleText;
	inline ImFont* HighlightText;
	inline ImFont* SubTitleText;
	inline ImFont* ChineseText;
	inline ImFont* NotifyText;
	inline ImFont* NotifyContentText;
	inline ImFont* IconText;
	inline ImFont* BigIconText;
	inline ImFont* LogoIconText;
	inline ImFont* TahomaText;
	inline ImFont* NotoSans;
}

struct FontConfig
{
	bool ReloadFont = false;
	float FontProportion = 1.f;

	float NormalFontSize = 15.f;
	float TitleFontSize = 16.f;
	float SubTitleFontSize = 15.f;
	float HighlightFontSize = 13.f;
	float ChineseFontSize = 16.f;

	float BaseSize = 25.f;
	float IconFontSize = BaseSize;
	float BigIconFontSize = BaseSize * (float)1.5;
	float NotifyFontSize = BaseSize * (float)1.2;
	float NotifyContentFontSize = BaseSize;
};
inline FontConfig FontConf;