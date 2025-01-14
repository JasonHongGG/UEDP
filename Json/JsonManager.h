#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "json.hpp"




class JsonManager
{
public:
	std::string SaveFilePath = "./json/ObjectData.json";
	std::ofstream SaveFile;
	std::ifstream OpenFile;

	std::vector<float> ImVec4ToVec(ImVec4 Obj) {
		return { Obj.x, Obj.y, Obj.z, Obj.w };
	};

	ImVec4 VecToImVec4(std::vector<float> Obj) {
		return ImVec4(Obj[0], Obj[1], Obj[2], Obj[3]);
	};

	// Save File
	void DeleteMenuSettingFile(std::string DeleteKey);

	void LoadMenuSettingFile(std::string LoadKey);

	void ReadMenuSettingFile();

	void WriteMenuSettingFile();


	//Api File
	void ReadAPIFile();

	void WriteAPIFile();


	// Example (Sample)
	void Process() {
		// json 使用方式
		std::ifstream jsonFileStream("./json/Test.json");
		nlohmann::json jsonData = nlohmann::json::parse(jsonFileStream);
		std::cout << std::setw(4) << jsonData << std::endl;


		//jsonData["name"] = 456;
		jsonData["Test"]["Name"] = "MyMan";
		jsonData["Arr"] = nlohmann::json::array();
		jsonData["Arr"].push_back(5);
		jsonData["Arr"].push_back("a");
		jsonData["Obj"] = nlohmann::json::object();
		jsonData["Obj"]["a"] = "A";
		jsonData["Obj"]["b"] = "B";

		std::ofstream jsonOutput("./json/Output.json");
		jsonOutput << std::setw(4) << jsonData << std::endl;
	}


private:
};

inline JsonManager JsonMgr = JsonManager();
