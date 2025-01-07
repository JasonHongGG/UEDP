#pragma once
#include <string>

struct NotificationConfig
{
	enum class NotiyType {
		None = 0,
		Success,
		Warning,
		Error,
		Info,
	};
	float DismissTime = 10;
	float DismissTimeBar = 2;	// �ݩ� event �ƥ�]�ˡA�]�����}�Ѽ�
	NotiyType Type = NotificationConfig::NotiyType::None;
	std::string Title = "";
	std::string Content = "";

	// ���ب�ưѼ�
	float DescendingSpeed = 10;
	float DecaySpeed = 0.25;
};
inline NotificationConfig NotificationConf;