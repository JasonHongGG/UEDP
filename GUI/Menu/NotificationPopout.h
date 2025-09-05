#pragma once
#include "Include/Notify/imgui_notify.h"
#include "Config/NotificationConfig.h"

namespace NotificationPopout
{
	void Render() {
		if (NotificationState.NotifyEvent == ProcessState::Start) {
			NotificationState.NotifyEvent = ProcessState::Processing;
			ImGuiToastType_ Type = ImGuiToastType_None;
			if (NotificationConf.Type == NotificationConfig::NotiyType::None)			Type = ImGuiToastType_None;
			else if (NotificationConf.Type == NotificationConfig::NotiyType::Success)	Type = ImGuiToastType_Success;
			else if (NotificationConf.Type == NotificationConfig::NotiyType::Warning)	Type = ImGuiToastType_Warning;
			else if (NotificationConf.Type == NotificationConfig::NotiyType::Error)		Type = ImGuiToastType_Error;
			else if (NotificationConf.Type == NotificationConfig::NotiyType::Info)		Type = ImGuiToastType_Info;
			ImGuiToast toast(Type, NotificationConf.DismissTime);
			toast.set_title(NotificationConf.Title.c_str());
			toast.set_content(NotificationConf.Content.c_str());
			ImGui::InsertNotification(toast);
			NotificationState.NotifyEvent = ProcessState::Completed;
		}

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
		ImGui::RenderNotifications(Font::NotifyText, Font::NotifyContentText);
		ImGui::PopStyleColor();
	}
}