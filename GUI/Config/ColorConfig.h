#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"

namespace Color
{
	inline ImVec4 ColorToVec(const float r, const float g, const float b, const float a)
	{
		return ImVec4(r / 255, g / 255, b / 255, a / 255);
	}

	// Original
	static inline ImVec4 Green = Color::ColorToVec(75, 201, 171, 255);	//藍綠色
	static inline ImVec4 LightGreen = Color::ColorToVec(0, 255, 127, 255);
	static inline ImVec4 Orange = Color::ColorToVec(255, 140, 0, 255);
	static inline ImVec4 Gray = Color::ColorToVec(169, 169, 169, 255);
	static inline ImVec4 White = Color::ColorToVec(255, 255, 255, 255);		//White
	static inline ImVec4 Black = Color::ColorToVec(0, 0, 0, 255);
	static inline ImVec4 DarkYellow = Color::ColorToVec(255, 213, 0, 255);
	static inline ImVec4 DarkPink = Color::ColorToVec(221, 160, 221, 255);

	// bround soft color
	static inline ImVec4 LightskyBlue = Color::ColorToVec(135, 206, 250, 255);
	static inline ImVec4 SoftBlue = Color::ColorToVec(161, 191, 229, 255);
	static inline ImVec4 LightBlue = Color::ColorToVec(58, 169, 255, 255);
	static inline ImVec4 SoftYellow = Color::ColorToVec(243, 241, 226, 255);
	static inline ImVec4 FluorescentYellow = Color::ColorToVec(255, 255, 0, 100); // 螢光黃


	// Component
	static inline ImVec4 SpinnerColor = LightskyBlue;		//lightskyblue
	static inline ImVec4 ProgressBarColor = LightBlue;	//softBlue
	static inline ImVec4 ProgressBarBGColor = SoftBlue;	//lightBlue
	static inline ImVec4 TabColor = Black;	//Black
	static inline ImVec4 TextColor = White;		//White
	static inline ImVec4 ObjectColor = LightskyBlue;		//lightskyblue
	static inline ImVec4 ObjectClickableColor = Green;
	static inline ImVec4 TypeColor = LightskyBlue;		//lightskyblue
	static inline ImVec4 NameColor = DarkYellow;		//yellow
	static inline ImVec4 OffestColor = DarkPink;	//varPink
	static inline ImVec4 ObjectAddressColor = White;		//White
	static inline ImVec4 AddressColor = White;		//White
	static inline ImVec4 ValueColor = White;		//White
	static inline ImVec4 TreeNodeHoverColor = Color::ColorToVec(19, 132, 202, 100);		//透明天空藍
	static inline ImVec4 TreeNavLineColor = White;		//White
	static inline ImVec4 HighlightColor = FluorescentYellow;


	static inline ImVec4 GradientColor1 = Black;
	static inline ImVec4 GradientColor2 = SoftYellow;
}