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
	float DismissTimeBar = 2;	// 妮 event ㄆン杆だ秨把计
	NotiyType Type = NotificationConfig::NotiyType::None;
	std::string Title = "";
	std::string Content = "";

	// ずㄧ计把计
	float DescendingSpeed = 10;
	float DecaySpeed = 0.25;
};
inline NotificationConfig NotificationConf;