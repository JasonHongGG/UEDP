#pragma once
#include "StorageManager.h"
#include "../Core/Object.h"
#include "../../GUI/Config/MainConsoleConfig.h"

void StorageManager::SetPackageDataList(std::vector<std::string> Value) { PackageViwerConf.PackageDataList = Value; }

bool StorageManager::SetObjectUper(DWORD_PTR Address) {
	if (!ObjectDict.Empty()) {
		for (auto Obj : ObjectDict.GetAll()) {
			if (Obj.second.SuperPtr.Address == Address) {
				BasicObjectData BasicObjData;
				ObjectMgr.BasicObjectDataWapper(Obj.second, BasicObjData);
				ObjectData tempObject = ObjectDict.Get(Address);
				tempObject.Uper.push_back(BasicObjData);
				ObjectDict.Set(Address, tempObject);
			}
		}
		return true;
	}
	else return false;
}

void StorageManager::SetObjectDict(DWORD_PTR Address, ObjectData& ObjData) 
{
	ObjectDictByAddress.Set(Address, ObjData.Address);
	ObjectDict.Set(Address, ObjData);
}

bool StorageManager::GetObjectDict(DWORD_PTR Address, ObjectData& ObjData, bool TryParseIfNoFind) {
	if (ObjectDictByAddress.Empty()) return false;
	if (ObjectDict.Empty()) return false;
	if (ObjectDictByAddress.Contains(Address)) {
		ObjData = ObjectDict.Get(Address); // 在 ObjectDict 中找到
		return true;
	}

	//沒找到，但嘗試再 Parse 一次
	if (TryParseIfNoFind) {
		ObjectData TempObjectData;
		if (!ObjectMgr.TrySaveObject(Address, TempObjectData, ObjectMgr.MaxLevel)) return false;
		ObjData = TempObjectData;
		return true;
	}

	return false;
}

bool StorageManager::GetObjectDictByID(size_t ID, ObjectData& ObjData) {
	if (ObjectDictByID.Empty()) return false;
	if (!ObjectDictByID.Contains(ID)) return false;
	if (ObjectDict.Empty()) return false;
	ObjData = ObjectDict.Get(ObjectDictByID.Get(ID));
	return true;
}

bool StorageManager::GetObjectDictByName(std::string Name, ObjectData& ObjData) {
	if (ObjectDictByName.Empty()) return false;
	if (!ObjectDictByName.Contains(Name)) return false;
	if (ObjectDict.Empty()) return false;
	ObjData = ObjectDict.Get(ObjectDictByName.Get(Name));
	return true;
}

bool StorageManager::GetObjectDictByFullName(std::string FullName, ObjectData& ObjData) {
	if (ObjectDictByFullName.Empty()) return false;
	if (!ObjectDictByFullName.Contains(FullName)) return false;
	if (ObjectDict.Empty()) return false;
	ObjData = ObjectDict.Get(ObjectDictByFullName.Get(FullName));
	return true;
}

std::map< std::string, std::map< std::string, std::map< std::string, std::pair<std::string, DWORD_PTR>>>>& StorageManager::GetPackage() { return Package; }
void StorageManager::SetPackage(DWORD_PTR Address, std::string PackageName, std::string Type, std::string ObjectName, std::string OuterObjectName) {
	Package[PackageName][Type][ObjectName + "#" + Utils.HexToString(Address)] = std::make_pair(ObjectName, Address);
	if (Type == "Function")
		Package[PackageName]["FunctionWithObject"][OuterObjectName + "#" + Utils.HexToString(Address)] = std::make_pair(OuterObjectName, Address);
}

void StorageManager::Reset() {
	UEVersion.UnIsInitialized();
	GWorldBaseAddress.UnIsInitialized();
	FNamePoolBaseAddress.UnIsInitialized();
	FNamePoolFirstPoolOffest.UnIsInitialized();
	FNamePoolFirstStringOffest.UnIsInitialized();
	FNamePoolSize.UnIsInitialized();
	FNamePoolParseExecutedFlag.UnIsInitialized();
	FNamePoolParseProgressBarValue.UnIsInitialized();
	FNamePoolParseProgressBarTotalValue.UnIsInitialized();
	FNamePoolFindStringFlag.UnIsInitialized();
	FNamePoolDict.Clear();
	GUObjectArrayBaseAddress.UnIsInitialized();
	GUObjectArrayElementSize.UnIsInitialized();
	GUObjectArrayFindObjectAddressByFullName.UnIsInitialized();
	GUObjectArrayParseIndex.UnIsInitialized();
	GUObjectArrayParseProgressBarValue.UnIsInitialized();
	GUObjectArrayParseProgressBarTotalValue.UnIsInitialized();
	GUObjectArrayTotalObjectCounter.UnIsInitialized();
	ObjectDict.Clear();
	ObjectDictByAddress.Clear();
	ObjectDictByID.Clear();
	ObjectDictByName.Clear();
	ObjectDictByFullName.Clear();
	ObjectTypeList.Clear();
	ObjectGraphUperProgressBarValue.UnIsInitialized();
	ObjectGraphUperProgressBarTotalValue.UnIsInitialized();
	Package.clear();
}