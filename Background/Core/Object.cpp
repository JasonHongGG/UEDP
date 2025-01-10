#include "Object.h"
#include "../Storage/StorageManager.h"


DWORD_PTR CorrectAddressDeviation(DWORD_PTR Address)
{
	// 目前似乎只有 Class 會需要
	size_t remainder = Address % 4;
	return Address - remainder;
}

bool ObjectManager::BasicObjectDataWapper(ObjectData& PropertyObject, BasicObjectData& TempBasicObjectData)
{
	TempBasicObjectData.ObjectID = PropertyObject.ObjectID;
	TempBasicObjectData.Type = PropertyObject.Type;
	TempBasicObjectData.Name = PropertyObject.Name;
	TempBasicObjectData.FullName = PropertyObject.FullName;
	TempBasicObjectData.Address = PropertyObject.Address;
	TempBasicObjectData.Offset = PropertyObject.Offset;
	return true;
}

bool ObjectManager::GetBasicInfo_1(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Outer)
{
	//ID
	ObjectId = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.ObjectID);

	//Outer
	Outer = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Outer);

	//Type
	DWORD_PTR Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.MemberTypeOffest);
	int type = MemMgr.MemReader.ReadMem<int>(Address_Level_1 + UEOffset.MemberType);
	if (type) return false;	//確認 Address_Level_1 確實是一個 Address  
	FNameParser.GetFNameStringByID(type, Type, true);
	if (Type.empty()) return false;		//假設只是形式上像 Address，結果該區塊又剛好是有效記憶體區塊，但只要不是正確位置仍然會讀不到字串 EX : 000002C400000043

	//Name
	int name = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.MemberFNameIndex);			// "None" Object 、"Member" Object 會走進這裡
	FNameParser.GetFNameStringByID(name, Name, true);
	if (Name.empty()) return (!Type.empty()) ? true : false;		// 如果 type 有，name 卻是空的，表示當前確實是 member 只是 name offset 是錯的，需要 autoConfig 去修正

	return true;
}

bool ObjectManager::GetBasicInfo_2(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Class, DWORD_PTR& Outer)
{
	//Class
	Class = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Class);

	//Type
	int type = MemMgr.MemReader.ReadMem<int>(Class + UEOffset.FNameIndex);
	if (type) return false;			// 確認 Class 確實是一個 Address
	FNameParser.GetFNameStringByID(type, Type, true);

	//Name
	int name = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.FNameIndex);
	FNameParser.GetFNameStringByID(name, Name, true);

	//ID
	ObjectId = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.ObjectID);

	//Outer
	Outer = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Outer);

	return true;
}

bool ObjectManager::GetBasicInfo(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level, DWORD_PTR ClassPtr)
{
	if (!GetBasicInfo_1(Address, RetObjectData.ObjectID, RetObjectData.Name, RetObjectData.Type, RetObjectData.Outer)) {			//簡單來說 GetBasicInfo_1 是給特殊 Object 走的，一半狀況則會走到 GetBasicInfo_2
		GetBasicInfo_2(Address, RetObjectData.ObjectID, RetObjectData.Name, RetObjectData.Type, ClassPtr, RetObjectData.Outer);
	}
	RetObjectData.Address = Address;
	if (RetObjectData.Name.empty()) RetObjectData.Name = "InvalidName";
	if (RetObjectData.Name.empty() or RetObjectData.Type.empty() or RetObjectData.Type.length() > 100) return false;		//異常資料處理
	return true;
}

void ObjectManager::GetFullName(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	int ConcateOuterCnt = 0;
	int MaxConcateOuterCnt = 10;
	// 串接 Parent 和 Child 之間的關係，並將解結果儲存在 Child，表示一路從底層走過來的 FullPath
	// 在取得 FullName 的同時，也是在解析所有 Parent Object
	ObjectData NewObj, OldObj;
	std::string TempStr = RetObjectData.Name;
	NewObj = RetObjectData;
	while (true) {
		if (ConcateOuterCnt >= MaxConcateOuterCnt) break;
		ConcateOuterCnt++;

		OldObj = NewObj;
		if (NewObj.Outer == NULL or !TrySaveObject(NewObj.Outer, NewObj, Level - 1)) break;		//如果 Outer != NULL，就會去做 TrySaveObject 取得 NewObj，兩步驟其中一步驟失敗就 break

		//是參數(注意!!!是往前串接，所以才可這樣寫)
		if ((OldObj.Type.find("Property") != std::string::npos || OldObj.Type.find("Function") != std::string::npos) &&
			(NewObj.Type.find("Property") == std::string::npos && NewObj.Type.find("Function") == std::string::npos))
			TempStr = NewObj.Name + ":" + TempStr;

		//還是 Property 或 Function
		else
			TempStr = NewObj.Name + "." + TempStr;
	}
	RetObjectData.FullName = TempStr;
}

bool ObjectManager::PropertyProcess(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	ObjectData PropertyObject;
	BasicObjectData TempBasicObjectData;
	if (TrySaveObject(Address, PropertyObject, Level - 1)) {
		BasicObjectDataWapper(PropertyObject, TempBasicObjectData);		//打包 PropertyObject 的基本資訊給 TempBasicObjectData
		RetObjectData.Property.push_back(TempBasicObjectData);			//當前返回物件包 RetObjectData 的 Property 新增 TempBasicObjectData 物件包

		//有 SubType 
		if (PropertyObject.Property.empty())			// 物件裡面就是SubType物件
			RetObjectData.SubType.push_back(PropertyObject.Address);
		else										// 物件裡面又包另一個物件
			RetObjectData.SubType.push_back(PropertyObject.Property[0].Address);
		return true;
	}
	else return false;
}

void ObjectManager::GetProperty(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	int16_t Offset;
	int16_t PropSize;
	int BitMask = 0;		//相關要給 Return Object 的數值都要初始化
	DWORD_PTR Property_0 = NULL, Property_8 = NULL, TypeObject = NULL;
	ObjectData PropertyObject;

	//讀取 Offset、Propsize
	Offset = MemMgr.MemReader.ReadMem<int16_t>(Address + UEOffset.Offset);
	RetObjectData.Offset = Offset;
	PropSize = MemMgr.MemReader.ReadMem<int16_t>(Address + UEOffset.PropSize);
	RetObjectData.PropSize = PropSize;

	//準備相關數據
	Property_0 = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Property);
	Property_8 = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Property + ProcessInfo::ProcOffestAdd);
	TypeObject = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.TypeObject);
	BitMask = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.BitMask);

	//在物件中，如果有以下這些特殊的變數，則將這些變數都視為一個 Object 再繼續作解析
	// Class Type 為 StructProperty、ObjectProperty、ClassProperty、Enum、Array
	// 1 沒 Property 就取 TypeObject
	// 2 有 Property 就取 Property
	if (RetObjectData.Type.find("StructProperty") != std::string::npos or
		RetObjectData.Type.find("ObjectProperty") != std::string::npos or
		RetObjectData.Type.find("ClassProperty") != std::string::npos or
		RetObjectData.Type.find("ArrayProperty") != std::string::npos or
		RetObjectData.Type.find("EnumProperty") != std::string::npos or
		RetObjectData.Type.find("ByteProperty") != std::string::npos	// ByteProperty 可能延伸到 EnumProperty
		)
	{
		// Is Property (Map's Member)
		if (PropertyProcess(RetObjectData, Property_8, Level)) {}
		// Is Property
		else if (PropertyProcess(RetObjectData, Property_0, Level)) {}
		// Is Type Object
		else if (PropertyProcess(RetObjectData, TypeObject, Level)) {}
	}
	// Class Type 為 MapProperty
	else if (RetObjectData.Type.find("MapProperty") != std::string::npos) {
		if (TrySaveObject(Property_8, PropertyObject, Level - 1)) {
			PropertyProcess(RetObjectData, Property_0, Level);
			PropertyProcess(RetObjectData, Property_8, Level);
		}
		else if (TrySaveObject(Property_0, PropertyObject, Level - 1)) {
			PropertyProcess(RetObjectData, TypeObject, Level);
			PropertyProcess(RetObjectData, Property_0, Level);
		}
	}
	// Class Type 為 BoolProperty
	else if (RetObjectData.Type.find("BoolProperty") != std::string::npos) {
		RetObjectData.BitMask = BitMask;

	}
}

void ObjectManager::GetMember(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	int MemberSize = 0;
	DWORD_PTR MemberAddress = NULL;
	ObjectData MemberObject;
	BasicObjectData TempBasicObjectData;
	MemberAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Member);
	if (TrySaveObject(MemberAddress, MemberObject, Level - 1, true)) {
		//取得 Member 、 MemberSize
		BasicObjectDataWapper(MemberObject, TempBasicObjectData);
		RetObjectData.MemberPtr = TempBasicObjectData;
		RetObjectData.MemberSize = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.Member + ProcessInfo::ProcOffestAdd);
	}
}

void ObjectManager::GetFunction(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	RetObjectData.Funct = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Funct);
}

std::string ObjectManager::ExtractPackageName(const std::string& input) {
	std::string result = "";

	// 找到二個 '/'
	size_t firstSlashPos = input.find('/');
	if (firstSlashPos == std::string::npos) return "";
	size_t secondSlashPos = input.find('/', firstSlashPos + 1);
	if (secondSlashPos == std::string::npos) return "";

	//3個的順序很重要，不能變
	size_t thirdSlashPos = input.find('/', secondSlashPos + 1);
	if (thirdSlashPos != std::string::npos) return input.substr(firstSlashPos, thirdSlashPos - firstSlashPos);

	size_t dotPos = input.find('.', secondSlashPos + 1);
	if (dotPos != std::string::npos) return input.substr(firstSlashPos, dotPos - firstSlashPos);

	size_t colonPos = input.find(':', secondSlashPos + 1);
	if (colonPos != std::string::npos) return input.substr(firstSlashPos, colonPos - firstSlashPos);

	result = input;
	return result;
}


void ObjectManager::SetPackage(ObjectData& RetObjectData)
{
	std::string Type = "";
	if (RetObjectData.FullName.find("/") != std::string::npos) {
		if (RetObjectData.Type.find("Struct") != std::string::npos or RetObjectData.Type.find("Class") != std::string::npos or RetObjectData.Type.find("Function") != std::string::npos or RetObjectData.Type.find("Enum") == 0) {
			std::string PackageName = ExtractPackageName(RetObjectData.FullName);
			std::string ObjectName = RetObjectData.Name;
			if (RetObjectData.Type.find("Struct") != std::string::npos or RetObjectData.Type.find("struct") != std::string::npos) Type = "Struct";
			else if (RetObjectData.Type.find("Class") != std::string::npos or RetObjectData.Type.find("class") != std::string::npos) Type = "Class";
			else if (RetObjectData.Type.find("Function") != std::string::npos or RetObjectData.Type.find("function") != std::string::npos) Type = "Function";
			else if (RetObjectData.Type.find("Enum") != std::string::npos or RetObjectData.Type.find("enum") != std::string::npos) Type = "Enum";

			std::string OuterObjectName = "";
			if (Type == "Function") {
				int NameIdx = MemMgr.MemReader.ReadMem<int>(RetObjectData.Outer + UEOffset.FNameIndex);			// "None" Object 、"Member" Object 會走進這裡
				FNameParser.GetFNameStringByID(NameIdx, OuterObjectName, true);
				OuterObjectName = "[ " + OuterObjectName + " ] " + ObjectName;
			}

			if (!RetObjectData.Name.empty())
				StorageMgr.SetPackage(RetObjectData.Address, PackageName, Type, ObjectName, OuterObjectName);
		}
	}
}

bool ObjectManager::TrySaveObject(DWORD_PTR Address, ObjectData& ObjData, size_t Level, bool SkipGetFullName, bool SearchMode)
{
	ObjectData TempObjectData;
	ObjectData RetObjectData;
	BasicObjectData TempBasicObjectData;
	DWORD_PTR Address_Level_1 = NULL, Address_Level_2 = NULL;

	//確認 Address 是一個 Pointer
	if (MemMgr.MemReader.ReadMem<DWORD_PTR>(Address)) return false;

	//查看該 Address 是否已經處理過，若處理過則直接返回結果
	if (StorageMgr.GetObjectDict(Address, TempObjectData)) { ObjData = TempObjectData; return true; }

	// Get Basic Info
	DWORD_PTR ClassPtr = NULL;		// <= 紀錄 Object 的 Class Address 之後再處理 (因為有 Level 深度的問題)
	if (GetBasicInfo(RetObjectData, Address, Level, ClassPtr) == false) return false;
	if (!RetObjectData.Type.empty()) StorageMgr.ObjectTypeList.Set(RetObjectData.Type, RetObjectData.Type);	// 儲存 Type 類型
	if (RetObjectData.Name == "None") { ObjData = RetObjectData; return true; }			// 如果是 None 則直接回傳 BasicInfo，且不儲存紀錄該物件
	if (RetObjectData.Name == "InvalidName") { ObjData = RetObjectData; return true; };

	// Get Full Name
	if (!SkipGetFullName and !Utils.ContainStr(RetObjectData.Type, "Property") and RetObjectData.Address != RetObjectData.Outer)
		GetFullName(RetObjectData, Address, Level);


	// 這邊主要是避免 Overflow，只要次數用完就取得基本資訊直接回傳，不儲存，後續進來該物件時再繼續解析
	if (Level >= MaxLevel) { ObjData = RetObjectData; return true; }

	//做第一次的物件存檔，加快執行流程
	{
		//檢查是否 Object ID 已存在於 Object Table ，若存在則直接返回結果，否則儲存進 Object Table (By ID)			//僅儲存主物件 //非 Property 物件
		if (RetObjectData.Type.find("Property") == std::string::npos) {		//不是 Member
			if (StorageMgr.GetObjectDictByID(Address, TempObjectData)) { ObjData = TempObjectData; return true; }

			if (RetObjectData.ObjectID < 0xFFFFFFFF) {
				StorageMgr.ObjectDictByID.Set(RetObjectData.ObjectID, RetObjectData.Address);		//By ID
			}
		}

		//儲存所有物件，不分類型
		if (StorageMgr.GetObjectDict(Address, TempObjectData)) { ObjData = TempObjectData; return true; }
		else StorageMgr.ObjectDict.Set(Address, RetObjectData);		//By Address

		//儲存所有物件 (Name)
		std::string FullName = Utils.rStringToLower(RetObjectData.FullName);
		if (!StorageMgr.GetObjectDictByName(RetObjectData.Name, TempObjectData)) StorageMgr.ObjectDictByName.Set(RetObjectData.Name, RetObjectData.Address);	//By FullName
		if (!StorageMgr.GetObjectDictByFullName(FullName, TempObjectData)) StorageMgr.ObjectDictByFullName.Set(FullName, RetObjectData.Address);	//By FullName
	}

	// 如果是搜尋模式，則取得基本資訊就返回
	if (SearchMode)
		return true;

	//Package Name 處理
	SetPackage(RetObjectData);

	StorageMgr.GUObjectArrayTotalObjectCounter.Set(StorageMgr.GUObjectArrayTotalObjectCounter.Get() + 1);		// Object Counter Add 1
	//--到這邊為止每個 ObjectData 的基本一定會有的屬性處理好了
	//-- 相關 List / Table 有以下 3 個
	//-- ObjectTable        Object ID
	//-- ObjectLists        Object Pointer
	//-- FullNameList       Object FullName

	//===============================================================================================================
	// 如果存在 Class 物件，則繼續處理
	ObjectData ClassObject;
	if (ClassPtr != NULL) {
		if (TrySaveObject(ClassPtr, ClassObject, Level - 1)) {
			// 這邊的 class.Instance 就是紀錄整個程式中有那些 Object 會使用到該 class
			BasicObjectDataWapper(ClassObject, TempBasicObjectData);
			RetObjectData.ClassPtr = TempBasicObjectData;			//把地址給 Pointer
		}
	}

	//	-- 注意!!!!搞清楚 super 、 Outer 各是甚麼
	//	-- super 是該 Object 的類別 / 型態，不論是本身的class或是繼承的class
	//	-- Outer 是該 Object 的父 Object，該父 Object 跟 Object 是兩個不同的東西，只是 父Object 有使用 Object 而已，Object 並沒有使用 父Object 的任何參數，也沒任何繼承
	DWORD_PTR Super;
	ObjectData SuperObject;
	if (UEOffset.UEOffestIsExist["Super"]) {		//如果設為 0 就關閉功能
		Super = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Super);
		if (TrySaveObject(Super, SuperObject, Level - 1)) {
			// 用 Uper 去紀錄有哪些 Object 會使用到該 父類別(Super)
			BasicObjectDataWapper(SuperObject, TempBasicObjectData);
			RetObjectData.SuperPtr = TempBasicObjectData;		//RetObjectData 紀錄 SuperObject
		}
	}

	//是 Property Object
	if (RetObjectData.Type.find("Property") != std::string::npos) {
		// Offset 存在才執行，所以設為 0 等於關閉此選項
		//取得 Offset、Propsize、(Property or BitMask)
		if (UEOffset.UEOffestIsExist["Offset"] and UEOffset.UEOffestIsExist["Property"] and UEOffset.UEOffestIsExist["PropSize"] and UEOffset.UEOffestIsExist["BitMask"])
			GetProperty(RetObjectData, Address, Level);

	}
	// 其餘的 Object 都嘗試去取得 Member，如果有的話
	else {
		// 取得 Member、MemberSize 
		if (UEOffset.UEOffestIsExist["Member"] and UEOffset.UEOffestIsExist["NextMember"])
			GetMember(RetObjectData, Address, Level);
	}



	//是 Function
	//如果遇到 Function 物件，則動態找尋 function 的 Offset
	if (RetObjectData.Type.find("Function") != std::string::npos) {
		if (UEOffset.UEOffestIsExist["Funct"])
			GetFunction(RetObjectData, Address, Level);
	}

	// 第一次進來的 Obejct 一定會完整走過一趟才會 Return True
	StorageMgr.ObjectDict.Set(Address, RetObjectData);		//By Address  //這時候儲存的就是完整個資訊

	//成功 !! 最終結果回傳
	//if (RetObjectData.FullName.find("Engine.") != std::string::npos) printf("[ FName FullName ] %p %s\n", RetObjectData.Address, RetObjectData.FullName.c_str());
	ObjData = RetObjectData;
	return true;
}