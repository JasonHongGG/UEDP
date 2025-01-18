// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <cmath> // ���F�ϥ� exp ���
#include <queue>
#include "../../../font/IconsFontAwesome6.h"
#include "../../../imgui/imgui.h"
#include "../../../imgui/imgui_internal.h"
#include "../../MyGuiComponent.h"


#define NOTIFY_MAX_MSG_LENGTH			4096		// Max message content length
#define NOTIFY_PADDING_X				10.f		// Bottom-left X padding
#define NOTIFY_PADDING_Y				10.f		// Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y		10.f		// Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME			0.1f			// Fade in and out duration	// ���
#define NOTIFY_FADE_IN_TIME				0.3f		// Fade in
#define NOTIFY_FADE_OUT_TIME			0.1f		// Fade out
#define NOTIGY_CLEAR_BUFFER_TIME		0.2F		// ���� Notify �������J�ù����
#define NOTIFY_DEFAULT_DISMISS			2		// Auto dismiss after X ms (default, applied only of no data provided in constructors)
#define NOTIFY_OPACITY					1.0f		// 0-1 Toast opacity
#define NOTIFY_TOAST_FLAGS				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing
// Comment out if you don't want any separator between title and content
//#define NOTIFY_USE_SEPARATOR

#define NOTIFY_INLINE					inline
#define NOTIFY_NULL_OR_EMPTY(str)		(!str ||! strlen(str))
#define NOTIFY_FORMAT(fn, format, ...)	if (format) { va_list args; va_start(args, format); fn(format, args, ##__VA_ARGS__); va_end(args); }

typedef int ImGuiToastType;
typedef int ImGuiToastPhase;
typedef int ImGuiToastPos;

enum ImGuiToastType_
{
	ImGuiToastType_None,
	ImGuiToastType_Success,
	ImGuiToastType_Warning,
	ImGuiToastType_Error,
	ImGuiToastType_Info,
	ImGuiToastType_COUNT	// �]�w�ɽu�A�ˬd�i��
};

enum ImGuiToastPhase_
{
	ImGuiToastPhase_FadeIn,
	ImGuiToastPhase_Wait,
	ImGuiToastPhase_FadeOut,
	ImGuiToastPhase_Expired,
	ImGuiToastPhase_COUNT
};

enum ImGuiToastPos_
{
	ImGuiToastPos_TopLeft,
	ImGuiToastPos_TopCenter,
	ImGuiToastPos_TopRight,
	ImGuiToastPos_BottomLeft,
	ImGuiToastPos_BottomCenter,
	ImGuiToastPos_BottomRight,
	ImGuiToastPos_Center,
	ImGuiToastPos_COUNT
};



class ImGuiToast
{
public:
	static double	global_last_creatobj_time;
	float			cur_height = -1;
	float			target_height = 0;
	double			real_time = 0;
	double			creation_time = 0;

private:
	ImGuiToastType	type = ImGuiToastType_None;
	char			title[NOTIFY_MAX_MSG_LENGTH];
	char			content[NOTIFY_MAX_MSG_LENGTH];
	double			dismiss_time = NOTIFY_DEFAULT_DISMISS;


	struct ToastColorsStruct
	{
		ImVec4 ImGuiToastType_None = { 255, 255, 255, 255 };			// White
		ImVec4 ImGuiToastType_Success = { 0, 255, 0, 255 };			// Green
		ImVec4 ImGuiToastType_Warning = { 255, 255, 0, 255 };		// Yellow
		ImVec4 ImGuiToastType_Error = { 255, 0, 0, 255 };			// Error
		ImVec4 ImGuiToastType_Info = { 0, 157, 255, 255 };			// Blue
		ImVec4 ImGuiToastType_Default = { 255, 255, 255, 255 };		// White
	};
	ToastColorsStruct ToastColors;

	struct ToastIconsStruct
	{
		const char* ImGuiToastType_None = NULL;
		const char* ImGuiToastType_Success = ICON_FA_CHECK_CIRCLE;
		const char* ImGuiToastType_Warning = ICON_FA_EXCLAMATION_TRIANGLE;
		const char* ImGuiToastType_Error = ICON_FA_TIMES_CIRCLE;
		const char* ImGuiToastType_Info = ICON_FA_INFO_CIRCLE;
		const char* ImGuiToastType_Default = NULL;
	};
	ToastIconsStruct ToastIcons;

private:
	// Setters

	NOTIFY_INLINE auto set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }

	NOTIFY_INLINE auto set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }

public:

	NOTIFY_INLINE auto set_title(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_title, format); }

	NOTIFY_INLINE auto set_content(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_content, format); }

	NOTIFY_INLINE auto set_type(const ImGuiToastType& type) -> void { IM_ASSERT(type < ImGuiToastType_COUNT); this->type = type; };

public:
	// Getters

	NOTIFY_INLINE auto get_title() -> char* { return this->title; };

	NOTIFY_INLINE auto get_default_title() -> const char*
	{
		if (!strlen(this->title))
		{
			switch (this->type)
			{
			case ImGuiToastType_None:
				return NULL;
			case ImGuiToastType_Success:
				return "Success";
			case ImGuiToastType_Warning:
				return "Warning";
			case ImGuiToastType_Error:
				return "Error";
			case ImGuiToastType_Info:
				return "Info";
			default:
				return NULL;
			}
		}

		return this->title;
	};

	NOTIFY_INLINE auto get_type() -> const ImGuiToastType& { return this->type; };

	NOTIFY_INLINE auto get_color() -> const ImVec4
	{
		switch (this->type)
		{
		case ImGuiToastType_None:
			return ToastColors.ImGuiToastType_None;
		case ImGuiToastType_Success:
			return ToastColors.ImGuiToastType_Success;
		case ImGuiToastType_Warning:
			return ToastColors.ImGuiToastType_Warning;
		case ImGuiToastType_Error:
			return ToastColors.ImGuiToastType_Error;
		case ImGuiToastType_Info:
			return ToastColors.ImGuiToastType_Info;
		default:
			return ToastColors.ImGuiToastType_Default; 
		}
	}

	NOTIFY_INLINE auto get_icon() -> const char*
	{
		switch (this->type)
		{
		case ImGuiToastType_None:
			return ToastIcons.ImGuiToastType_None;
		case ImGuiToastType_Success:
			return ToastIcons.ImGuiToastType_Success;
		case ImGuiToastType_Warning:
			return ToastIcons.ImGuiToastType_Warning;
		case ImGuiToastType_Error:
			return ToastIcons.ImGuiToastType_Error;
		case ImGuiToastType_Info:
			return ToastIcons.ImGuiToastType_Info;
		default:
			return ToastIcons.ImGuiToastType_Default;
		}
	}

	NOTIFY_INLINE auto get_content() -> char* { return this->content; };

	NOTIFY_INLINE auto get_phase() -> const ImGuiToastPhase
	{
		const double elapsed = ImGui::GetTime() - this->creation_time;
		const double real_elapsed = ImGui::GetTime() - this->real_time;

		// �i���ɦҼ{���U���s���ɶ�
		if (real_elapsed <= NOTIFY_FADE_IN_TIME)	
		{
			return ImGuiToastPhase_FadeIn;
		}

		// ��l�h�Ҽ{�T������������ (����T�����h���ɶ��i�H���}�A�Y�϶i���ɶ��ۦP)
		else if (elapsed > NOTIFY_FADE_IN_TIME + this->dismiss_time + NOTIFY_FADE_OUT_TIME + NOTIGY_CLEAR_BUFFER_TIME)
		{
			return ImGuiToastPhase_Expired;
		}
		else if (elapsed > NOTIFY_FADE_IN_TIME + this->dismiss_time)
		{
			return ImGuiToastPhase_FadeOut;
		}
		else
		{
			return ImGuiToastPhase_Wait;
		}

	}

	NOTIFY_INLINE auto get_fade_percent() -> const float
	{
		const auto phase = get_phase();
		double elapsed = ImGui::GetTime() - this->creation_time;
		if (elapsed < 0) elapsed = 0;

		if (phase == ImGuiToastPhase_FadeIn)
		{
			return (float)((double)elapsed / (double)NOTIFY_FADE_IN_TIME) * NOTIFY_OPACITY;
		}
		else if (phase == ImGuiToastPhase_FadeOut)
		{
			// elapsed - NOTIFY_FADE_IN_OUT_TIME - this->dismiss_time ��ܹL�����ɶ�
			return (float)(1.f - (((double)elapsed - (double)NOTIFY_FADE_IN_TIME - (double)this->dismiss_time) / (double)NOTIFY_FADE_OUT_TIME)) * NOTIFY_OPACITY;
		}

		return 1.f * NOTIFY_OPACITY;
	}

	NOTIFY_INLINE auto get_fade_xpos(float StrWidth) -> const float
	{
		const auto phase = get_phase();
		if (phase == ImGuiToastPhase_FadeIn)
		{
			double elapsed = ImGui::GetTime() - this->real_time;	//�i���ɶȦҼ{���U���s���ɨ�

			return (float)(1.f - ((double)elapsed / (double)NOTIFY_FADE_IN_TIME)) * StrWidth;		//�Ʀr�v���W�h�A������ê�������֡A�v���X��	(1-�ƭ�)
		}
		else if (phase == ImGuiToastPhase_FadeOut)
		{
			double elapsed = ImGui::GetTime() - this->creation_time;	// �h���ɦҼ{�ɶ�����

			// elapsed - NOTIFY_FADE_IN_TIME - this->dismiss_time ��ܹL�����ɶ�
			return (float)(((double)elapsed - (double)NOTIFY_FADE_IN_TIME - (double)this->dismiss_time) / (double)NOTIFY_FADE_OUT_TIME) * StrWidth;		//���ê������ܦh (�ƭ�) 
		}	

		return 0;
	}

	NOTIFY_INLINE auto height_update(int loop_idx, float height)
	{
		if (this->cur_height == -1) {
			this->cur_height = height;
			this->target_height = height;
		}
		if (this->cur_height != height)
			this->target_height = height;
		if (this->cur_height != this->target_height) {
			//if (this->cur_height - this->target_height >= 270)
			//	this->cur_height -= 20;
			this->cur_height -= (float)8 * std::exp(-0.25 * loop_idx);	//�v���w�M
			//current_toast->cur_height -= TestCompMgr.FloatSilder1 / (loop_idx + 1);		//���t

			// �̤p�W��
			if (this->cur_height < this->target_height) this->cur_height = this->target_height;
		}
	}


public:
	// Constructors

	ImGuiToast(ImGuiToastType type, double dismiss_time = NOTIFY_DEFAULT_DISMISS)
	{
		// �ˬd type �O�_���T
		IM_ASSERT(type < ImGuiToastType_COUNT);

		// ��l�ưѼ�
		this->type = type;
		this->dismiss_time = dismiss_time;

		// �M��
		memset(this->title, 0, sizeof(this->title));
		memset(this->content, 0, sizeof(this->content));
	}

	ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type) { NOTIFY_FORMAT(this->set_content, format); }

	ImGuiToast(ImGuiToastType type, double dismiss_time, const char* format, ...) : ImGuiToast(type, dismiss_time) { 
		NOTIFY_FORMAT(this->set_content, format);	// content �榡�� EX: "String is %s", myStr
	}
};

inline double ImGuiToast::global_last_creatobj_time = 0;

namespace ImGui
{
	NOTIFY_INLINE std::vector<ImGuiToast> notifications;
	NOTIFY_INLINE std::queue<ImGuiToast> notifications_queue;
	NOTIFY_INLINE void InsertNotification(ImGuiToast& toast)
	{
		notifications_queue.push(toast);
	}
	NOTIFY_INLINE void RemoveNotification(int index)
	{
		notifications.erase(notifications.begin() + index);
	}

	/// <summary>
	/// Render toasts, call at the end of your rendering!
	/// </summary>
	NOTIFY_INLINE void RenderNotifications(ImFont* TitleFont, ImFont* ContentFont)
	{
		// �]�w�k�U����m
		ImVec2 vp_size = GetMainViewport()->Size;
		float height = 0.f;

		// push queue to notifications list
		while (notifications.size() < 7 and notifications_queue.size() > 0)
		{
			// �� queue ���ƶ����T���K�[�� notifications list
			notifications.push_back(notifications_queue.front());
			notifications_queue.pop();

			// �T�O�C�� Item �b�����ɦ������ɶ����^ (���|�P�ɨ�ӰT�����^) (�b�s�W�ɳ]�w real_time (���F fade in), �H�� creation_time (���F���d�ɶ��H�� fade out))
			double CurTime = ImGui::GetTime();
			notifications[notifications.size() - 1].real_time = CurTime;
			if (CurTime - ImGuiToast::global_last_creatobj_time < 1)
				notifications[notifications.size() - 1].creation_time = CurTime + (1 - (CurTime - ImGuiToast::global_last_creatobj_time));
			else
				notifications[notifications.size() - 1].creation_time = CurTime;
			ImGuiToast::global_last_creatobj_time = notifications[notifications.size() - 1].creation_time;
		}

		// for-loop notifications
		for (int i = 0; i < notifications.size(); i++)
		{
			ImGuiToast* current_toast = &notifications[i];

			if (current_toast->get_phase() == ImGuiToastPhase_Expired)
				continue;

			// Get icon, title and other data
			const char* icon = current_toast->get_icon();
			const char* title = current_toast->get_title();
			const char* content = current_toast->get_content();
			const char* default_title = current_toast->get_default_title();
			ImVec2 StrSize = (ImGui::CalcTextSize(icon) + ImGui::CalcTextSize(title)) > ImGui::CalcTextSize(content) ? (ImGui::CalcTextSize(icon) + ImGui::CalcTextSize(title)) : ImGui::CalcTextSize(content);
			float opacity = 1.f; 
			float xOffset = current_toast->get_fade_xpos(StrSize.x * 3 + NOTIFY_PADDING_X);
			ImVec4 text_color = current_toast->get_color();
			text_color.w = opacity;

			// ���ק�s
			current_toast->height_update(i, height);

			// Render
			ImGui::PushFont(TitleFont);
			SetNextWindowBgAlpha(opacity);
			SetNextWindowPos(ImVec2(vp_size.x - NOTIFY_PADDING_X + xOffset, vp_size.y - NOTIFY_PADDING_Y - current_toast->cur_height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
			Begin(("##TOAST " + std::to_string(i)).c_str(), NULL, NOTIFY_TOAST_FLAGS);
			{
				
				PushTextWrapPos(vp_size.x / 3.f); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width

				ImGui::InvisibleButton("vsplitter", ImVec2(4.0f, ImGui::CalcTextSize(title).y));
				ImGui::SameLine();
				{
					// icon
					if (!NOTIFY_NULL_OR_EMPTY(icon))
						ImGui::ShadowText(icon, text_color, text_color, 100);

					if (!NOTIFY_NULL_OR_EMPTY(icon))
						SameLine();

					// title
					if (!NOTIFY_NULL_OR_EMPTY(title))
						TextColored(text_color, title); // Render title text
					else 
						TextColored(text_color, (std::to_string(height) + " " + std::to_string(current_toast->cur_height) + " " + std::to_string(current_toast->target_height)).c_str()); // Render title text
						//Text(default_title); // Render default title text (ImGuiToastType_Success -> "Success", etc...)
				}
				ImGui::SameLine();
				ImGui::InvisibleButton("vsplitter", ImVec2(4.0f, ImGui::CalcTextSize(title).y));

				ImGui::InvisibleButton("vsplitter", ImVec2(4.0f, ImGui::CalcTextSize(content).y));
				ImGui::SameLine();
				{
					// content
					ImGui::PushFont(ContentFont);
					if (!NOTIFY_NULL_OR_EMPTY(content))
					{
						// In case ANYTHING was rendered in the top, we want to add a small padding so the text (or icon) looks centered vertically
						SetCursorPosY(GetCursorPosY() + 5.f);
#ifdef NOTIFY_USE_SEPARATOR
						Separator();
#endif
						Text(content); // Render content text
					}
					ImGui::PopFont();
				}
				ImGui::SameLine();
				ImGui::InvisibleButton("vsplitter", ImVec2(4.0f, ImGui::CalcTextSize(content).y));

				PopTextWrapPos();
			}

			// Save height for next toasts
			height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;
			End();
			ImGui::PopFont();
		}

		// �]�����S�O���}�C�ӰT���������ɶ��A�ҥH�令�Τ@���b�����ɳB�z�T��������
		if (notifications.size() > 0 and notifications[0].get_phase() == ImGuiToastPhase_Expired)
		{
			RemoveNotification(0);
		}
	}
}

#endif
