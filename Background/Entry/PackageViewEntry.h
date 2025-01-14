#pragma once
#include <shared_mutex>
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../Storage/StorageManager.h"
#include "../../System/Memory.h"
#include "../Core/UEOffset.h"
#include "UtilsEntry.h"

namespace PackageViewEntry
{
	inline std::shared_mutex GetGlobalSearchObjectLock;

	void UpdataPackageObjectList();
	void CreatPackageObectTab();
	void UpdataGlobalSearchObject();
	void ShowGlobalSearchState();


	inline void Entry()
	{
		if (PackageViwerState.ObjectDataUpdateEvent.State == ProcessState::Start) {
			PackageViwerState.ObjectDataUpdateEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Updata Package Object List");
			UpdataPackageObjectList();
			PackageViwerState.ObjectDataUpdateEvent.State = ProcessState::Completed;
		}

		if (PackageViwerState.ObjectContentCreatEvent.State == ProcessState::Start) {
			PackageViwerState.ObjectContentCreatEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Creat Package Obect Tab");
			CreatPackageObectTab();
			PackageViwerState.ObjectContentCreatEvent.State = ProcessState::Completed;
		}

		if (PackageViwerState.SearchUpdateEvent.State == ProcessState::Start) {
			PackageViwerState.SearchUpdateEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Updata Global Search Object");
			UpdataGlobalSearchObject();
			PackageViwerState.SearchUpdateEvent.State = ProcessState::Completed;
		}

		if (PackageViwerState.SearchShowEvent.State == ProcessState::Start) {
			PackageViwerState.SearchShowEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Show Global Search State");
			ShowGlobalSearchState();
			PackageViwerState.SearchShowEvent.State = ProcessState::Completed;
		}
	}
}

void PackageViewEntry::UpdataPackageObjectList()
{
	PackageViwerConf.PackageObjectDataList = StorageMgr.GetPackage()[PackageViwerState.ObjectDataUpdateEvent.PackageName];
}


void PackageViewEntry::CreatPackageObectTab()
{
	ObjectData RetObjData;
	ObjectData TempObjData;

	DumperObject	PackageObject;
	BasicDumperObject TempBasicPackageObject;
	BasicDumperObject	TempSubTypeObject;

	DWORD_PTR Address_Level_1 = NULL, TempAddress = NULL;

	// 檢查是否已存在
	for (int i = 0; i < PackageViwerConf.ObjectContentTabNameList.size(); i++) {
		if (PackageViwerConf.ObjectContentTabNameList[i].compare(PackageViwerState.ObjectContentCreatEvent.Name) == 0) return;
	}


	//基本資料
	StorageMgr.GetObjectDict(PackageViwerState.ObjectContentCreatEvent.Address, RetObjData);
	PackageObject.Address = RetObjData.Address;
	PackageObject.Type = RetObjData.Type;
	PackageObject.Name = RetObjData.Name;
	PackageObject.FullName = RetObjData.FullName;

	//Class 繼承的 Super List
	std::vector<BasicDumperObject> TempSuperList;
	Address_Level_1 = RetObjData.SuperPtr.Address;
	while (true) {
		if (StorageMgr.GetObjectDict(Address_Level_1, TempObjData)) {
			TempBasicPackageObject.Address = TempObjData.Address;;
			TempBasicPackageObject.Type = TempObjData.Type;
			TempBasicPackageObject.Name = TempObjData.Name;;
			TempSuperList.push_back(TempBasicPackageObject);
			Address_Level_1 = TempObjData.SuperPtr.Address;	//下一層 Super Class
		}
		else break;
	}
	PackageObject.Super = TempSuperList;

	// 如果是 Property Object 從下面開始讀取的是 Property Object 指向的 Object (Object 的內容)
	if (RetObjData.Type.find("ObjectProperty") != std::string::npos) {
		// Property 有東西
		if (!RetObjData.Property.empty())
			StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
		// TypeObject 才有東西
		else {
			Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
			StorageMgr.GetObjectDict(Address_Level_1, RetObjData);
		}
	}
	else if (RetObjData.Type.find("StructProperty") != std::string::npos) {
		// 確認是否有 Property， 有 Property 就取出 Object
		if (!RetObjData.Property.empty()) {
			PackageObject.ObjectAddress = RetObjData.Property[0].Address;
			StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
		}
		// 否則就取 TypeObject
		else {
			Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
			StorageMgr.GetObjectDict(Address_Level_1, TempObjData);

			// 如果 ScriptStruct 有 Member 則直接回傳該物件
			if (TempObjData.MemberPtr.Address != NULL) {
				PackageObject.ObjectAddress = TempObjData.Address;
				RetObjData = TempObjData;
			}
			// 如果 ScriptStruct 有 Property
			else if (!TempObjData.Property.empty()) {
				PackageObject.ObjectAddress = TempObjData.Property[0].Address;
				StorageMgr.GetObjectDict(TempObjData.Property[0].Address, RetObjData);
			}
			// 如果 ScriptStruct 沒有 Property => 看 super
			else {
				PackageObject.ObjectAddress = TempObjData.SuperPtr.Address;
				StorageMgr.GetObjectDict(TempObjData.SuperPtr.Address, RetObjData);
			}
		}
	}
	// Enum List
	else if (RetObjData.Type.find("EnumProperty") != std::string::npos) {
		// Type
		Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
		if (StorageMgr.GetObjectDict(Address_Level_1, TempObjData, true))
			PackageObject.TypeObject = TempObjData.Type;

		PackageObject.ObjectAddress = RetObjData.Property[0].Address;
		StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
	}
	else if (Utils.rStringToLower(RetObjData.Type).find("property") != std::string::npos) {
		PackageObject.ObjectAddress = RetObjData.Property[0].Address;		//如果沒有預先儲存再 Property 表示不支援該型態
		StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
	}

	// Member List 
	if (RetObjData.Type.find("Function") == std::string::npos)
	{
		std::vector<BasicDumperObject> TempMemberList;
		Address_Level_1 = RetObjData.MemberPtr.Address;
		while (true) {
			ObjectData MemberObj;
			if (StorageMgr.GetObjectDict(Address_Level_1, MemberObj, true)) {
				// 基本檢查 (如果不符合就跳過該 Member)
				if (MemberObj.Offset < 0) {	//MemberObj.Type.find("Function") != std::string::npos or
					Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember); continue;
				}

				BasicDumperObject TempBasicPackageObject;
				TempBasicPackageObject.Address = MemberObj.Address;
				TempBasicPackageObject.Type = MemberObj.Type;
				TempBasicPackageObject.Name = MemberObj.Name;

				// SubType
				UtilsEntry::ObjectSubTypeProc(MemberObj.Address, TempBasicPackageObject, true);

				if (TempBasicPackageObject.Type.find("Property") == std::string::npos and UtilsEntry::IsObjectNameInPackage(TempBasicPackageObject.Type, TempAddress)) TempBasicPackageObject.Clickable = true;
				TempMemberList.push_back(TempBasicPackageObject);

				//下一個 Member
				Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember);
			}
			else break;
		}
		PackageObject.Member = TempMemberList;
	}


	// Enum List
	if (RetObjData.Type.find("Enum") != std::string::npos) {
		std::vector<std::pair<std::string, size_t>> TempEnumList;
		UtilsEntry::GetEnumList(RetObjData.Address, TempEnumList);
		PackageObject.Enum = TempEnumList;
	}


	// Function 
	if (RetObjData.Type.find("Function") != std::string::npos) {
		PackageObject.Funct.Address = RetObjData.Funct;
		PackageObject.Funct.Offset = (RetObjData.Funct > 0) ? RetObjData.Funct - (size_t)ProcessModule::hMainMoudle : 0;

		// Belong To What Class
		ObjectData FunctClassObj;		// 其實就是 Outer
		if (StorageMgr.GetObjectDict(RetObjData.Outer, FunctClassObj, true)) {
			BasicDumperObject TempBasicObject;
			TempBasicObject.Address = FunctClassObj.Address;
			TempBasicObject.Type = FunctClassObj.Type;
			TempBasicObject.Name = FunctClassObj.Name;
			TempBasicObject.Clickable = true;	//是物件必定可點擊
			PackageObject.Funct.Outer = TempBasicObject;
		}

		// Paramter
		std::vector<BasicDumperObject> TempFunctParaList;
		Address_Level_1 = RetObjData.MemberPtr.Address;
		while (true) {
			ObjectData ParaObj;
			if (StorageMgr.GetObjectDict(Address_Level_1, ParaObj, true)) {
				BasicDumperObject TempBasicParaObject;
				TempBasicParaObject.Address = ParaObj.Address;
				TempBasicParaObject.Type = ParaObj.Type;
				TempBasicParaObject.Name = ParaObj.Name;

				// SubType
				UtilsEntry::ObjectSubTypeProc(ParaObj.Address, TempBasicParaObject, true);

				// Type Is Click ?
				if (TempBasicParaObject.Type.find("Property") == std::string::npos and
					UtilsEntry::IsObjectNameInPackage(TempBasicParaObject.Type, TempAddress))
					TempBasicParaObject.Clickable = true;

				// Save
				if (ParaObj.Name == "ReturnValue") {	// 最後一個 parameter
					PackageObject.Funct.Ret = TempBasicParaObject;
					break;
				}
				else TempFunctParaList.push_back(TempBasicParaObject);

				//下一個 Member
				Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(ParaObj.Address + UEOffset.NextPara);
			}
			else break;
		}
		PackageObject.Funct.Para = TempFunctParaList;
		if (PackageObject.Funct.Ret.Address == 0x0) PackageObject.Funct.Ret.Name = "Void";	// 如果 Para 沒有 Return Type
	}


	//儲存最終結果
	PackageViwerConf.ObjectContentMap[PackageViwerState.ObjectContentCreatEvent.Name] = PackageObject;
	//加入 TabNameList
	PackageViwerConf.ObjectContentTabOpenState[PackageViwerState.ObjectContentCreatEvent.Address] = true;
	PackageViwerConf.ObjectContentTabNameList.push_back(PackageViwerState.ObjectContentCreatEvent.Name);
}


void PackageViewEntry::UpdataGlobalSearchObject()
{
	PackageViwerConf.GlobalSearchList.clear();
	for (auto& NamesEntry : StorageMgr.GetPackage()) {
		for (auto& TypeEntry : NamesEntry.second) {
			if (TypeEntry.first == "Function") continue;	// Function 只查 FunctionWithObject 就好，才會有對應的 ObjectName
			for (auto& Object : TypeEntry.second) {
				if (PackageViwerConf.GlobalSearchMode == PackageViwerConfig::GlobalSearchMode::ObjectName) {
					if (Utils.rStringToLower(Object.first).find(Utils.rStringToLower(PackageViwerState.SearchUpdateEvent.SearchStr)) != std::string::npos) {
						PackageViwerConfig::GlobalSearchObject TempGlobalSearchObject;
						TempGlobalSearchObject.PackageName = NamesEntry.first;
						TempGlobalSearchObject.TabName = TypeEntry.first;
						TempGlobalSearchObject.ObjectName = Object.second.first;
						TempGlobalSearchObject.Address = Object.second.second;
						std::string ShowName = TempGlobalSearchObject.ObjectName;
						PackageViwerConf.GlobalSearchList[ShowName] = TempGlobalSearchObject;
					}
				}
				else if (PackageViwerConf.GlobalSearchMode == PackageViwerConfig::GlobalSearchMode::MemberName) {
					ObjectData Obj;
					StorageMgr.GetObjectDict(Object.second.second, Obj);

					if (Obj.MemberPtr.Address != NULL) {
						DWORD_PTR Address_Level_1 = Obj.MemberPtr.Address;
						while (Address_Level_1) {			// Address_Level_1 != NULL
							ObjectData MemberObj;
							if (StorageMgr.GetObjectDict(Address_Level_1, MemberObj, true)) {
								// 基本檢查 (如果不符合就跳過該 Member)
								if (MemberObj.Offset < 0) {
									Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember); continue;
								}

								// 名稱比對
								if (Utils.rStringToLower(MemberObj.Name).find(Utils.rStringToLower(PackageViwerState.SearchUpdateEvent.SearchStr)) != std::string::npos) {
									PackageViwerConfig::GlobalSearchObject TempGlobalSearchObject;
									TempGlobalSearchObject.PackageName = NamesEntry.first;
									TempGlobalSearchObject.TabName = TypeEntry.first;
									TempGlobalSearchObject.ObjectName = Object.second.first;
									TempGlobalSearchObject.Address = Object.second.second;
									TempGlobalSearchObject.MemberName = MemberObj.Name;
									std::string ShowName = TempGlobalSearchObject.ObjectName + " [ " + MemberObj.Name + " ]";
									PackageViwerConf.GlobalSearchList[ShowName] = TempGlobalSearchObject;
								}

								//下一個 Member
								Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember);
							}
							else break;
						}
					}
				}
			}
		}
	}
}


void PackageViewEntry::ShowGlobalSearchState()
{
	// 會調用到 PackageObjectData(ObjectCateory)，需要先把 PackageObjectData 鎖起來才能避免兩條線程同時使用到的問題
	GetGlobalSearchObjectLock.lock();

	// clear serch string
	if (PackageViwerConf.PackageDataListFilterStr[0] != '\0')
		PackageViwerConf.PackageDataListFilterStr[0] = '\0';
	if (PackageViwerConf.PackageObjectDataListFilterStr[0] != '\0')
		PackageViwerConf.PackageObjectDataListFilterStr[0] = '\0';

	// Package List Index 、 PackageObjectList
	for (int i = 0; i < PackageViwerConf.PackageDataList.size(); i++) {
		if (PackageViwerConf.PackageDataList[i].find(PackageViwerState.SearchShowEvent.PackageName) != std::string::npos) {
			PackageViwerConf.PackageObjectDataList = StorageMgr.GetPackage()[PackageViwerState.SearchShowEvent.PackageName];
		}
	}

	// Package List Scroll Y Pos
	PackageViwerConf.SetPackageDataListScrollYPos = PackageViwerState.SearchShowEvent.PackageName;

	// PackageObject Tab Flag
	for (auto& TabFlagMap : PackageViwerConf.PackageObjectDataTabFlagMap) TabFlagMap.second = ImGuiTabItemFlags_None;
	PackageViwerConf.PackageObjectDataTabFlagMap[PackageViwerState.SearchShowEvent.TabName] = ImGuiTabItemFlags_SetSelected;

	// Object List Scroll Y Pos
	PackageViwerConf.SetObjectDataListScrollYPos = PackageViwerState.SearchShowEvent.ObjectName;

	// Show Object Content [Send Event]
	EventHandler::OpenPackageObjectTab(PackageViwerState.SearchShowEvent.ObjectName, PackageViwerState.SearchShowEvent.Address);
	GetGlobalSearchObjectLock.unlock();
}
