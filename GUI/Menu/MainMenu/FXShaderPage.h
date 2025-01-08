#pragma once
#include <string>
#include <vector>
#include "../../../imgui/imgui.h"
#include "../../Config/MainMenuConfig.h"

#include "FXShader/FXShaderManager.h"
#include "FXShader/ConnectionLineEffect.h"
#include "FXShader/RainEffect.h"
#include "FXShader/BubbleEffect.h"
#include "FXShader/IconFollowerEffect.h"

#include "../../TextureLoader.h"
#include "../../MyGuiComponent.h"

namespace FXShaderPage
{
	void Render(MainMenuCurPage PageID)
	{
		if (PageID != MainMenuCurPage::FX) return;

		ImGui::BeginChild("FX", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
		{
			const auto& mp = ImGui::GetMousePos();
			const auto& p = ImGui::GetWindowPos();
			const auto& sz = ImGui::GetWindowSize();
			const auto& pWindowDrawList = ImGui::GetWindowDrawList();

			ImVec2 Max = { p.x + sz.x, p.y + sz.y };
			static bool FX_Connection_Check;
			static bool FX_Rain_Check;
			static bool FX_Bubble_Check;
			static bool FX_IconFollower_Check;
			ImGui::Checkbox("FX_Connection", &FX_Connection_Check);
			ImGui::SameLine(); static int ConnectThreshod = 400, SelectColorThreshold = 200;
			ImGui::PushItemWidth(50); ImGui::DragInt("Connect Threshod", &ConnectThreshod); ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PushItemWidth(50); ImGui::DragInt("Color Threshold", &SelectColorThreshold); ImGui::PopItemWidth();

			ImGui::Checkbox("FX_Rain", &FX_Rain_Check);
			ImGui::Checkbox("FX_Bubble", &FX_Bubble_Check);
			ImGui::Checkbox("FX_IconFollower_Check", &FX_IconFollower_Check);
			ImGui::SameLine(); static float FollowSpeed = 1.f;
			ImGui::PushItemWidth(50); ImGui::DragFloat("Follow Speed", &FollowSpeed); ImGui::PopItemWidth();

			if (FX_Connection_Check) 
				FXShaderMgr.AddEffect(std::make_unique<ConnectionLineEffect>(ConnectThreshod, SelectColorThreshold));
			if (FX_Rain_Check) 
				FXShaderMgr.AddEffect(std::make_unique<RainEffect>());
			if (FX_Bubble_Check) 
				FXShaderMgr.AddEffect(std::make_unique<BubbleEffect>());
			if (FX_IconFollower_Check and ImageLoader.ImageTextureVec.size() > 0) 
				FXShaderMgr.AddEffect(std::make_unique<IconFollowerEffect>(ImageLoader.ImageTextureVec[ImageLoader.FrameDisplayCnt], ImVec2((float)ImageLoader.ImageWidth, (float)ImageLoader.ImageHeight), FollowSpeed));
		
			FXShaderMgr.RenderAll(pWindowDrawList, { p, Max, sz, mp, (float)ImGui::GetTime() });
			FXShaderMgr.ClearEffect();
		}
		ImGui::EndChild();
	}
}