#include "JsonManager.h"
#include "../System/Process.h"
#include "../Utils/Utils.h"
#include "../Gui/Menu.h"
#include "../State/EventHandler.h"
#include "../State/GUIState.h"
#include "../GUI/Config/ColorConfig.h"
#include "../GUI/Config/StyleConfig.h"
#include "../GUI/Config/MainConsoleConfig.h"
#include "../GUI/Config/LoadAndSaveSettingConfig.h"

class SaveParaClass {
public:
	// ====== Dumper ======
	float* ObjectInfoWindoWidth = &MainConsoleConf.ObjectInfoWindoWidth;
	float* InputTextWidth = &MainConsoleConf.InputTextWidth;
	float* DumperTableWidth = &MainConsoleConf.DumperTableWidth;
	float* DumperTableHeight = &MainConsoleConf.DumperTableHeight;
	float* DragAreaWidth = &MainConsoleConf.DragAreaWidth;

	// ====== PackageViwer ======
	float* PackageData_Width = &PackageViwerConf.PackageDataWindowWidth;
	float* PackageObject_Width = &PackageViwerConf.PackageObjectWindowWidth;
	float* ObjectTypeIndentation = &PackageViwerConf.ObjectTypeIndentation;
	float* ObjectNameindentation = &PackageViwerConf.ObjectNameindentation;

	// ====== Inspector ======
	int* IndentationIndependent = &InspectorConf.IndentationIndependent;
	int* IndentationAccumulate = &InspectorConf.IndentationAccumulate;
	int* AlignGroupObjectIndentation = &InspectorConf.AlignGroupObjectIndentation;
	float* InspectorList_Width = &InspectorConf.InspectorList_Width;

	float* DerivedList_Width = &InspectorConf.DerivedList_Width;
	float* InspectorBaseIndentation = &InspectorConf.InspectorBaseIndentation;
	float* InspectorOffsetIndentation = &InspectorConf.InspectorOffsetIndentation;
	float* InspectorTypeIndentation = &InspectorConf.InspectorTypeIndentation;

	float* InspectorNameIndentation = &InspectorConf.InspectorNameIndentation;
	float* InspectorObjectAddressIndentation = &InspectorConf.InspectorObjectAddressIndentation;
	float* InspectorAddressIndentation = &InspectorConf.InspectorAddressIndentation;
	float* InspectorValueIndentation = &InspectorConf.InspectorValueIndentation;

	float* InspectorTreeNodePadding = &Style::TreeNodePadding;
	int* InspectorTreeNavLine = &InspectorConf.InspectorTreeNavLine;
	float* InspectorTreeNavLineOffset = &InspectorConf.InspectorTreeNavLineOffset;
	float* InspectorTreeNavBranchLineSize = &InspectorConf.InspectorTreeNavBranchLineSize;


	// ====== Color ======
	ImVec4* TabColor = &Color::TabColor;
	ImVec4* TextColor = &Color::TextColor;
	ImVec4* ObjectColor = &Color::ObjectColor;
	ImVec4* ObjectClickableColor = &Color::ObjectClickableColor;

	ImVec4* TypeColor = &Color::TypeColor;
	ImVec4* NameColor = &Color::NameColor;
	ImVec4* ObjectAddressColor = &Color::ObjectAddressColor;
	ImVec4* AddressColor = &Color::AddressColor;

	ImVec4* ValueColor = &Color::ValueColor;
	ImVec4* OffestColor = &Color::OffestColor;
	ImVec4* TreeNodeHoverColor = &Color::TreeNodeHoverColor;
	ImVec4* TreeNavLineColor = &Color::TreeNavLineColor;

	SaveParaClass() {};

	std::map<std::string, std::function<DWORD_PTR()>> getFloatMembers() const {
		return {
			// ====== Dumper ======
			{"ObjectInfoWindoWidth", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectInfoWindoWidth); }},
			{"InputTextWidth", [this]() { return reinterpret_cast<DWORD_PTR>(this->InputTextWidth); }},
			{"DumperTableWidth", [this]() { return reinterpret_cast<DWORD_PTR>(this->DumperTableWidth); }},
			{"DumperTableHeight", [this]() { return reinterpret_cast<DWORD_PTR>(this->DumperTableHeight); }},
			{"DragAreaWidth", [this]() { return reinterpret_cast<DWORD_PTR>(this->DragAreaWidth); }},

			// ====== PackageViwer ======
			{"PackageData_Width", [this]() { return reinterpret_cast<DWORD_PTR>(this->PackageData_Width); }},
			{"PackageObject_Width", [this]() { return reinterpret_cast<DWORD_PTR>(this->PackageObject_Width); }},
			{"ObjectTypeIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectTypeIndentation); }},
			{"ObjectNameindentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectNameindentation); }},
			
			// ====== Inspector ======
			{"InspectorList_Width", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorList_Width); }},

			{"DerivedList_Width", [this]() { return reinterpret_cast<DWORD_PTR>(this->DerivedList_Width); }},
			{"InspectorBaseIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorBaseIndentation); }},
			{"InspectorOffsetIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorOffsetIndentation); }},
			{"InspectorTypeIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorTypeIndentation); }},

			{"InspectorNameIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorNameIndentation); }},
			{"InspectorObjectAddressIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorObjectAddressIndentation); }},
			{"InspectorAddressIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorAddressIndentation); }},
			{"InspectorValueIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorValueIndentation); }},

			{"InspectorTreeNodePadding", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorTreeNodePadding); }},
			{"InspectorTreeNavLineOffset", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorTreeNavLineOffset); }},
			{"InspectorTreeNavBranchLineSize", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorTreeNavBranchLineSize); }},
		};
	}

	std::map<std::string, std::function<DWORD_PTR()>> getIntMembers() const {
		return {
			// ====== Inspector ======
			{"IndentationIndependent", [this]() { return reinterpret_cast<DWORD_PTR>(this->IndentationIndependent); }},
			{"IndentationAccumulate", [this]() { return reinterpret_cast<DWORD_PTR>(this->IndentationAccumulate); }},
			{"AlignGroupObjectIndentation", [this]() { return reinterpret_cast<DWORD_PTR>(this->AlignGroupObjectIndentation); }},
			{"InspectorTreeNavLine", [this]() { return reinterpret_cast<DWORD_PTR>(this->InspectorTreeNavLine); }},
		};
	}

	std::map<std::string, std::function<DWORD_PTR()>> getColorMembers() const {
		return {
			// ====== Color ======
			{"TabColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->TabColor); }},
			{"TextColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->TextColor); }},
			{"ObjectColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectColor); }},
			{"ObjectClickableColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectClickableColor); }},

			{"TypeColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->TypeColor); }},
			{"NameColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->NameColor); }},
			{"ObjectAddressColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->ObjectAddressColor); }},
			{"AddressColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->AddressColor); }},

			{"ValueColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->ValueColor); }},
			{"OffestColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->OffestColor); }},
			{"TreeNodeHoverColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->TreeNodeHoverColor); }},
			{"TreeNavLineColor", [this]() { return reinterpret_cast<DWORD_PTR>(this->TreeNavLineColor); }},
		};
	}
};
inline SaveParaClass SavePara = SaveParaClass();

void JsonManager::DeleteMenuSettingFile(std::string DeleteKey)
{
	nlohmann::json JsonData;
	std::ifstream OpenFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath);
	if (OpenFile) {
		if (OpenFile.peek() != std::ifstream::traits_type::eof())
			OpenFile >> JsonData;
		OpenFile.close();
	}

	if (JsonData.find(DeleteKey) != JsonData.end())
		JsonData.erase(DeleteKey);

	std::ofstream SaveFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath);
	if (SaveFile) {
		SaveFile << std::setw(4) << JsonData << std::endl;
		SaveFile.close();
	}
}

void JsonManager::LoadMenuSettingFile(std::string LoadKey)
{
	nlohmann::json JsonData;
	std::ifstream OpenFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath);
	if (OpenFile) {
		if (OpenFile.peek() == std::ifstream::traits_type::eof()) return;	//如果檔案是空的
		OpenFile >> JsonData;

		if (!JsonData.empty()) {
			nlohmann::json ObjJson = JsonData[LoadKey];

			for (const auto& member : SavePara.getFloatMembers()) {
				*(reinterpret_cast<float*>(member.second())) = ObjJson[member.first];
			}
			for (const auto& member : SavePara.getIntMembers()) {
				*(reinterpret_cast<int*>(member.second())) = ObjJson[member.first];
			}
			for (const auto& member : SavePara.getColorMembers()) {
				*(reinterpret_cast<ImVec4*>(member.second())) = VecToImVec4(ObjJson[member.first]);
			}
		}
	}
}

void JsonManager::ReadMenuSettingFile()
{
	nlohmann::json JsonData;
	std::ifstream OpenFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath);
	if (OpenFile) {
		if (OpenFile.peek() == std::ifstream::traits_type::eof()) return;	//如果檔案是空的
		OpenFile >> JsonData;

		if (!JsonData.empty()) {
			for (auto& [key, value] : JsonData.items()) {
				LoadAndSaveSettingConfig::SaveObject TempMenuSettingSaveStruct;
				TempMenuSettingSaveStruct.Title = value["Title"];
				TempMenuSettingSaveStruct.Description = value["Description"];
				TempMenuSettingSaveStruct.CurTime = value["CurTime"];

				LoadAndSaveSettingConf.SaveListConf.push_back(TempMenuSettingSaveStruct);
			}
		}
	}
}

void JsonManager::WriteMenuSettingFile()
{
	std::string Title = LoadAndSaveSettingState.SaveEvent.Title;
	std::string Description = LoadAndSaveSettingState.SaveEvent.Description;
	std::string CurTime = Utils.GetCurTime();

	// 檢查並讀取現有的 JSON 文件
	nlohmann::json JsonData;
	std::ifstream OpenFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath);
	if (OpenFile) {
		if (OpenFile.peek() != std::ifstream::traits_type::eof())
			OpenFile >> JsonData;
		OpenFile.close();
	}

	// 更新 JSON 數據
	JsonData[CurTime] = {
		{"Title", Title},
		{"Description", Description},
		{"CurTime", CurTime},
	};
		// 不同型態的參數處理
	for (const auto& member : SavePara.getFloatMembers()) {
		JsonData[CurTime][member.first] = *(reinterpret_cast<float*>(member.second()));
	}
	for (const auto& member : SavePara.getIntMembers()) {
		JsonData[CurTime][member.first] = *(reinterpret_cast<int*>(member.second()));
	}
	for (const auto& member : SavePara.getColorMembers()) {
		JsonData[CurTime][member.first] = ImVec4ToVec(*(reinterpret_cast<ImVec4*>(member.second())));
	}

		
	// 寫入
	std::ofstream SaveFile(LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath, std::ios::out | std::ios::trunc); // 使用覆蓋模式
	if (!SaveFile) {
		std::cerr << "Error: Unable to open file for writing at " << LoadAndSaveSettingState.SaveEvent.SaveJasonFilePath << std::endl;
		return;
	}

	SaveFile << std::setw(4) << JsonData << std::endl;
	SaveFile.close();


	// GUI Update
	LoadAndSaveSettingConfig::SaveObject TempMenuSettingSaveStruct;
	TempMenuSettingSaveStruct.Title = Title;
	TempMenuSettingSaveStruct.Description = Description;
	TempMenuSettingSaveStruct.CurTime = CurTime;
	LoadAndSaveSettingConf.SaveListConf.push_back(TempMenuSettingSaveStruct);
}












void JsonManager::ReadAPIFile()
{
	nlohmann::json JsonData;
	std::string ReadFilePath = APIState.OpenFileEvent.FilePath;
	std::ifstream OpenFile(ReadFilePath);
	if (OpenFile) {
		if (OpenFile.peek() == std::ifstream::traits_type::eof()) return;	//如果檔案是空的
		OpenFile >> JsonData;

		if (!JsonData.empty()) {
			for (auto& arrObj : JsonData["ObjectArray"]) {
				for (auto& [addrKey, objData] : arrObj.items()) {
					DumperItem TempAPIListObject;
					TempAPIListObject.ID = objData["ID"];
					TempAPIListObject.Name = objData["Name"];
					TempAPIListObject.Type = objData["RefObjName"];
					TempAPIListObject.FullName = objData["FullName"];
					TempAPIListObject.Type = objData["Type"];
					TempAPIListObject.InstanceAddress = objData["Address"];

					for (auto& [objInfoKey, objInfo] : objData["ObjectMap"].items()) {
						DumperObject TempDumperObject;
						TempDumperObject.Name = objInfo["Name"];
						TempDumperObject.InstanceAddress = objInfo["Address"];
						TempDumperObject.Offset = objInfo["Offset"];

						for (auto& objMember : objInfo["Member"]) {
							BasicDumperObject TempBasicDumperObject;
							TempBasicDumperObject.Name = objMember["Name"];
							TempBasicDumperObject.Type = objMember["Type"];
							TempBasicDumperObject.Offset = objMember["Offset"];
							TempBasicDumperObject.InstanceAddress = objMember["Address"];
							TempBasicDumperObject.Bit = objMember["Bit"];
							TempDumperObject.Member.push_back(TempBasicDumperObject);
						}
						TempAPIListObject.ObjectMap[TempDumperObject.InstanceAddress] = TempDumperObject;
					}
					APIConf.APIItemList.push_back(TempAPIListObject);
				}
			}
		}
	}
}

void JsonManager::WriteAPIFile()
{
	// 寫入就從新寫入，不要用 append，因為原本的參數可能會修改

	// file 基本資訊(時間、日期...)
	nlohmann::json JsonData;
	JsonData["ProcName"] = ProcessInfo::ProcessName;
	JsonData["WindowName"] = ProcessWindow::WindowName;
	JsonData["Time"] = Utils.GetCurTime();
	JsonData["ObjectArray"] = nlohmann::json::array();

	// 每個 header 為一小包
	for (int i = 0; i < APIConf.APIItemList.size(); i++) {
		// 每個一小包的個別基本資訊
		nlohmann::json ObjectPackage;
		DumperItem TempAPIListObject = APIConf.APIItemList[i];
		std::string keyStr = Utils.HexToString(TempAPIListObject.InstanceAddress);
		ObjectPackage[keyStr] = nlohmann::json::object();
		ObjectPackage[keyStr]["Name"] = TempAPIListObject.Name;
		ObjectPackage[keyStr]["RefObjName"] = TempAPIListObject.Type;
		ObjectPackage[keyStr]["FullName"] = TempAPIListObject.FullName;
		ObjectPackage[keyStr]["Type"] = TempAPIListObject.Type;
		ObjectPackage[keyStr]["ID"] = TempAPIListObject.ID;
		ObjectPackage[keyStr]["Address"] = TempAPIListObject.InstanceAddress;
		ObjectPackage[keyStr]["ObjectMap"] = nlohmann::json::object();

		// 每個一小包的 ObjectMap
		nlohmann::json& ObjectInfo = ObjectPackage[keyStr]["ObjectMap"];
		for (auto const& [key, obj] : TempAPIListObject.ObjectMap) {
			keyStr = Utils.HexToString(key);
			// ObjectMap 的每一小項，包含自己本身的資訊，以及含有的 member
			ObjectInfo[keyStr] = nlohmann::json::object();
			ObjectInfo[keyStr]["Name"] = obj.Name;
			ObjectInfo[keyStr]["Address"] = obj.InstanceAddress;
			ObjectInfo[keyStr]["Offset"] = obj.Offset;
			ObjectInfo[keyStr]["Member"] = nlohmann::json::array();

			for (int k = 0; k < obj.Member.size(); k++) {
				// 每個 member 又有自己的資訊
				nlohmann::json ObjectMember;
				BasicDumperObject TempBasicDumperObject = obj.Member[k];
				ObjectMember["Name"] = TempBasicDumperObject.Name;
				ObjectMember["Type"] = TempBasicDumperObject.Type;
				ObjectMember["Offset"] = TempBasicDumperObject.Offset;
				ObjectMember["Address"] = TempBasicDumperObject.InstanceAddress;
				ObjectMember["Bit"] = TempBasicDumperObject.Bit;
				ObjectInfo[keyStr]["Member"].push_back(ObjectMember);
			}
		}

		JsonData["ObjectArray"].push_back(ObjectPackage);
	}

	// 寫入
	std::string WriteFilePath = APIState.SaveFileEvent.FilePath;
	std::ofstream SaveFile(WriteFilePath, std::ios::out | std::ios::trunc); // 使用覆蓋模式
	if (!SaveFile) {EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "API", "Unable to open file for writing");return;}
	SaveFile << std::setw(4) << JsonData << std::endl;
	SaveFile.close();
}
