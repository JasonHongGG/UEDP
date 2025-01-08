#pragma once
#include "../../../imgui/imgui.h"
#include "../../Config/MainMenuConfig.h"
#include "../../Config/FontConfig.h"
#include "../../TextureLoader.h"
#include "../../MyGuiComponent.h"
#include "../../Overlay/D3DRender.h"

namespace ImagePage
{
	void StateUpdate()
	{
		ImageLoader.FrameTimer = (int)((float)ImageLoader.FrameTimer - ImGui::GetIO().DeltaTime);
		if (ImageLoader.FrameTimer <= 0) {
			ImageLoader.FrameTimer = 3;  //ImageDelays[FrameDisplayCnt] / 1000;		//delay is ms => ms / s => delay / 1000		// Update Speed
			ImageLoader.FrameDisplayCnt++;
			if (ImageLoader.FrameDisplayCnt >= ImageLoader.FrameCnt) ImageLoader.FrameDisplayCnt = 0;
		}
	}

	void Render(MainMenuCurPage PageID)
	{
		if (PageID != MainMenuCurPage::Image) return;
		static int DefaultImageWidth = 0;
		static int DefaultImageHeight = 0;
		static float ImageProportion = 0;
		static bool ProportionEnable = true;

		ImGui::BeginChild("ShowImage", ImVec2(0, 0), true);
		{
			// Title
			ImGui::PushFont(Font::TitleText);
			ImGui::Text("Show Image");
			ImGui::PopFont();

			// Load Image
			ImGui::DummySpace();
			static char ImagePathStr[128] = { "logo/capoo.gif" };		//head.PNG
			ImGui::PushItemWidth(100.f);
			ImGui::InputTextWithHint("##ImagePath", "Image Path", ImagePathStr, IM_ARRAYSIZE(ImagePathStr));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Load Image##ImageRender")) {
				bool ret = ImageLoader.LoadTextureFromFile(D3DRenderMgr.Device, ("./" + std::string(ImagePathStr)).c_str(), ImageLoader.ImageTextureVec, &ImageLoader.ImageWidth, &ImageLoader.ImageHeight, &ImageLoader.FrameCnt);
				ImageProportion = (float)ImageLoader.ImageWidth / (float)ImageLoader.ImageHeight;
				DefaultImageWidth = ImageLoader.ImageWidth;
				DefaultImageHeight = ImageLoader.ImageHeight;
				IM_ASSERT(ret);
			}
			ImGui::SameLine(); ImGui::Checkbox("Proportion Resize", &ProportionEnable);
			ImGui::SameLine(); if (ImGui::Button("Default")) { ImageLoader.ImageWidth = DefaultImageWidth; ImageLoader.ImageHeight = DefaultImageHeight; }

			// Resize Image
			ImGui::DragInt("ImageWidth", &ImageLoader.ImageWidth, 1);
			if (ProportionEnable && ImGui::IsItemEdited()) { ImageLoader.ImageHeight = static_cast<int>(std::round(ImageLoader.ImageWidth / ImageProportion)); }
			ImGui::DragInt("ImageHeight", &ImageLoader.ImageHeight, 1);
			if (ProportionEnable && ImGui::IsItemEdited()) { ImageLoader.ImageWidth = static_cast<int>(std::round(ImageLoader.ImageHeight * ImageProportion)); }
			ImGui::SliderInt("FrameDisplayCnt", &ImageLoader.FrameDisplayCnt, 0, ImageLoader.FrameCnt - 1);

			// Show Image
			if (ImageLoader.ImageTextureVec.size() > 0) {
				ImGui::Image((void*)ImageLoader.ImageTextureVec[ImageLoader.FrameDisplayCnt], ImVec2((float)ImageLoader.ImageWidth, (float)ImageLoader.ImageHeight));
			}

		}
		ImGui::EndChild();

		StateUpdate();
	}

}