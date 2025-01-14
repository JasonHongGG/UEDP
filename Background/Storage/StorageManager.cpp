#pragma once
#include "StorageManager.h"
#include "../Core/Object.h"

bool StorageManager::SetObjectUper(DWORD_PTR Address) {
	//std::shared_lock<std::shared_mutex> lock(GetObjectDict_Lock);

	if (!ObjectDict.Empty()) {
		for (auto Obj : ObjectDict.GetAll()) {
			if (Obj.second.SuperPtr.Address == Address) {
				BasicObjectData BasicObjData;
				ObjectMgr.BasicObjectDataWapper(Obj.second, BasicObjData);
				ObjectDict.Get(Address).Uper.push_back(BasicObjData);
			}
		}
		return true;
	}
	else return false;

}

bool StorageManager::GetObjectDict(DWORD_PTR Address, ObjectData& ObjData, bool TryParseIfNoFind) {
	if (ObjectDictByAddress.Empty()) return false;
	if (!ObjectDictByAddress.Contains(Address)) return false;
	if (ObjectDict.Empty()) return false;
	try {
		ObjData = ObjectDict.Get(Address); // 在 ObjectDict 中找到
		return true;
	}
	catch (std::string e) {
		//沒找到，但嘗試再 Parse 一次
		if (TryParseIfNoFind) {
			ObjectData TempObjectData;
			if (!ObjectMgr.TrySaveObject(Address, TempObjectData, ObjectMgr.MaxLevel)) return false;
			ObjData = TempObjectData;
			return true;
		}
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