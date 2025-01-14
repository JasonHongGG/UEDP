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

	// �ˬd�O�_�w�s�b
	for (int i = 0; i < PackageViwerConf.ObjectContentTabNameList.size(); i++) {
		if (PackageViwerConf.ObjectContentTabNameList[i].compare(PackageViwerState.ObjectContentCreatEvent.Name) == 0) return;
	}


	//�򥻸��
	StorageMgr.GetObjectDict(PackageViwerState.ObjectContentCreatEvent.Address, RetObjData);
	PackageObject.Address = RetObjData.Address;
	PackageObject.Type = RetObjData.Type;
	PackageObject.Name = RetObjData.Name;
	PackageObject.FullName = RetObjData.FullName;

	//Class �~�Ӫ� Super List
	std::vector<BasicDumperObject> TempSuperList;
	Address_Level_1 = RetObjData.SuperPtr.Address;
	while (true) {
		if (StorageMgr.GetObjectDict(Address_Level_1, TempObjData)) {
			TempBasicPackageObject.Address = TempObjData.Address;;
			TempBasicPackageObject.Type = TempObjData.Type;
			TempBasicPackageObject.Name = TempObjData.Name;;
			TempSuperList.push_back(TempBasicPackageObject);
			Address_Level_1 = TempObjData.SuperPtr.Address;	//�U�@�h Super Class
		}
		else break;
	}
	PackageObject.Super = TempSuperList;

	// �p�G�O Property Object �q�U���}�lŪ�����O Property Object ���V�� Object (Object �����e)
	if (RetObjData.Type.find("ObjectProperty") != std::string::npos) {
		// Property ���F��
		if (!RetObjData.Property.empty())
			StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
		// TypeObject �~���F��
		else {
			Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
			StorageMgr.GetObjectDict(Address_Level_1, RetObjData);
		}
	}
	else if (RetObjData.Type.find("StructProperty") != std::string::npos) {
		// �T�{�O�_�� Property�A �� Property �N���X Object
		if (!RetObjData.Property.empty()) {
			PackageObject.ObjectAddress = RetObjData.Property[0].Address;
			StorageMgr.GetObjectDict(RetObjData.Property[0].Address, RetObjData);
		}
		// �_�h�N�� TypeObject
		else {
			Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
			StorageMgr.GetObjectDict(Address_Level_1, TempObjData);

			// �p�G ScriptStruct �� Member �h�����^�ǸӪ���
			if (TempObjData.MemberPtr.Address != NULL) {
				PackageObject.ObjectAddress = TempObjData.Address;
				RetObjData = TempObjData;
			}
			// �p�G ScriptStruct �� Property
			else if (!TempObjData.Property.empty()) {
				PackageObject.ObjectAddress = TempObjData.Property[0].Address;
				StorageMgr.GetObjectDict(TempObjData.Property[0].Address, RetObjData);
			}
			// �p�G ScriptStruct �S�� Property => �� super
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
		PackageObject.ObjectAddress = RetObjData.Property[0].Address;		//�p�G�S���w���x�s�A Property ��ܤ��䴩�ӫ��A
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
				// ���ˬd (�p�G���ŦX�N���L�� Member)
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

				//�U�@�� Member
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
		ObjectData FunctClassObj;		// ���N�O Outer
		if (StorageMgr.GetObjectDict(RetObjData.Outer, FunctClassObj, true)) {
			BasicDumperObject TempBasicObject;
			TempBasicObject.Address = FunctClassObj.Address;
			TempBasicObject.Type = FunctClassObj.Type;
			TempBasicObject.Name = FunctClassObj.Name;
			TempBasicObject.Clickable = true;	//�O���󥲩w�i�I��
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
				if (ParaObj.Name == "ReturnValue") {	// �̫�@�� parameter
					PackageObject.Funct.Ret = TempBasicParaObject;
					break;
				}
				else TempFunctParaList.push_back(TempBasicParaObject);

				//�U�@�� Member
				Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(ParaObj.Address + UEOffset.NextPara);
			}
			else break;
		}
		PackageObject.Funct.Para = TempFunctParaList;
		if (PackageObject.Funct.Ret.Address == 0x0) PackageObject.Funct.Ret.Name = "Void";	// �p�G Para �S�� Return Type
	}


	//�x�s�̲׵��G
	PackageViwerConf.ObjectContentMap[PackageViwerState.ObjectContentCreatEvent.Name] = PackageObject;
	//�[�J TabNameList
	PackageViwerConf.ObjectContentTabOpenState[PackageViwerState.ObjectContentCreatEvent.Address] = true;
	PackageViwerConf.ObjectContentTabNameList.push_back(PackageViwerState.ObjectContentCreatEvent.Name);
}


void PackageViewEntry::UpdataGlobalSearchObject()
{
	PackageViwerConf.GlobalSearchList.clear();
	for (auto& NamesEntry : StorageMgr.GetPackage()) {
		for (auto& TypeEntry : NamesEntry.second) {
			if (TypeEntry.first == "Function") continue;	// Function �u�d FunctionWithObject �N�n�A�~�|�������� ObjectName
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
								// ���ˬd (�p�G���ŦX�N���L�� Member)
								if (MemberObj.Offset < 0) {
									Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember); continue;
								}

								// �W�٤��
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

								//�U�@�� Member
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
	// �|�եΨ� PackageObjectData(ObjectCateory)�A�ݭn���� PackageObjectData ��_�Ӥ~���קK����u�{�P�ɨϥΨ쪺���D
	GetGlobalSearchObjectLock.lock();

	// clear serch string
	if (PackageViwerConf.PackageDataListFilterStr[0] != '\0')
		PackageViwerConf.PackageDataListFilterStr[0] = '\0';
	if (PackageViwerConf.PackageObjectDataListFilterStr[0] != '\0')
		PackageViwerConf.PackageObjectDataListFilterStr[0] = '\0';

	// Package List Index �B PackageObjectList
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
