#pragma once
#include <Windows.h>
#include <map>
#include "../Interface/StorageInterface.h"
#include "../Interface/ObjectInterface.h"

class StorageManager
{
public:
	Property<size_t> UEVersion;
	Property<size_t> GWorldBaseAddress;

	Property<size_t> FNamePoolBaseAddress;
	Property<size_t> FNamePoolFirstPoolOffest;
	Property<size_t> FNamePoolFirstStringOffest;
	Property<size_t> FNamePoolSize;
	Property<bool> FNamePoolParseExecutedFlag;
	Property<size_t> FNamePoolParseProgressBarValue;
	Property<size_t> FNamePoolParseProgressBarTotalValue;
	Property<bool> FNamePoolFindStringFlag;

	PropertyMap<size_t, std::string> FNamePoolDict = PropertyMap<size_t, std::string>(true);



	Property<size_t> GUObjectArrayBaseAddress;
	Property<size_t> GUObjectArrayElementSize;
	Property<DWORD_PTR> GUObjectArrayFindObjectAddressByFullName;
	Property<size_t> GUObjectArrayParseIndex;
	Property<size_t> GUObjectArrayParseProgressBarValue;
	Property<size_t> GUObjectArrayParseProgressBarTotalValue;
	Property<size_t> GUObjectArrayTotalObjectCounter;
	
	PropertyMap<DWORD_PTR, ObjectData> ObjectDict = PropertyMap<DWORD_PTR, ObjectData>(true);
	PropertyMap<DWORD_PTR, DWORD_PTR> ObjectDictByAddress = PropertyMap<DWORD_PTR, DWORD_PTR>();
	PropertyMap<size_t, DWORD_PTR> ObjectDictByID;
	PropertyMap<std::string, DWORD_PTR> ObjectDictByName;
	PropertyMap<std::string, DWORD_PTR> ObjectDictByFullName;
	PropertyMap<std::string, std::string> ObjectTypeList;

	
	void SetPackageDataList(std::vector<std::string> Value);
	bool SetObjectUper(DWORD_PTR Address);
	void SetObjectDict(DWORD_PTR Address, ObjectData& ObjData);
	bool GetObjectDict(DWORD_PTR Address, ObjectData& ObjData, bool TryParseIfNoFind = false);
	bool GetObjectDictByID(size_t ID, ObjectData& ObjData);
	bool GetObjectDictByName(std::string Name, ObjectData& ObjData);
	bool GetObjectDictByFullName(std::string FullName, ObjectData& ObjData);

	std::map< std::string, std::map< std::string, std::map< std::string, std::pair<std::string, DWORD_PTR>>>>& GetPackage();
	void SetPackage(DWORD_PTR Address, std::string PackageName, std::string Type, std::string ObjectName, std::string OuterObjectName);
private:
	//PackageViewer
	std::map< std::string, std::map< std::string, std::map< std::string, std::pair<std::string, DWORD_PTR>> > > Package;
};
inline StorageManager StorageMgr = StorageManager();