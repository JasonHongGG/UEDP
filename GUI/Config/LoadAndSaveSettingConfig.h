#pragma once
#include <string>
#include <vector>

struct LoadAndSaveSettingConfig
{
	struct SaveObject {
		std::string CurTime = "";
		std::string Title = "";
		std::string Description = "";
		std::string FilePath = "./MenuSetting.json";
	};
	SaveObject SaveConf;
	std::vector<SaveObject> SaveListConf;
};
inline LoadAndSaveSettingConfig LoadAndSaveSettingConf;