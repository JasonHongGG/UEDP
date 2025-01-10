#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "UEOffset.h"
#include "Parser.h"
#include "../Interface/ObjectInterface.h"
#include "../../System/Memory.h"
#include "../../Utils/Include/BeaEngine.h"
#include "../../Utils/Utils.h"

class ObjectManager
{
public:


	bool BasicObjectDataWapper(ObjectData& PropertyObject, BasicObjectData& TempBasicObjectData);

	bool GetBasicInfo_1(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Outer);
	bool GetBasicInfo_2(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Class, DWORD_PTR& Outer);
	bool GetBasicInfo(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level, DWORD_PTR ClassPtr);

	void GetFullName(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	bool PropertyProcess(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetProperty(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetMember(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetFunction(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	std::string ExtractPackageName(const std::string& input);
	void SetPackage(ObjectData& RetObjectData);

	/*
	Address : Object 的 Address
	ObjData : 用來接收 Object Data 的參數，在 TrySaveObject 過程中會逐漸將解析的資料放入
	Level : 對大的遞迴層數
	(基本上超過 50 就會爆了)
	SkipGetFullName : 是否跳過 FullName 分析，主要是 Member Object 會需要跳過分析，因為若開發者設計不當這部分可能會出現無限迴圈
	(你的 FullName 是你自己之類的)
	SearchMode : 是否只是單存在搜尋
	( 此參數主要是在 Parse 前會使用，EX: AutoConfig 在設定參數時可能就會以某個 Object 來設定，
	  但是由於參數還不準確，因次當前分析的結果會捨棄，所以不執行完整解析，直接提早結束 )
	*/
	bool TrySaveObject(DWORD_PTR Address, ObjectData& ObjData, size_t Level, bool SkipGetFullName = false, bool SearchMode = false);

private:

public:
	size_t MaxLevel = 100;

};

inline ObjectManager ObjectMgr = ObjectManager();