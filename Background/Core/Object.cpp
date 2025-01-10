#include "Object.h"
#include "../Storage/StorageManager.h"


DWORD_PTR CorrectAddressDeviation(DWORD_PTR Address)
{
	// �ثe���G�u�� Class �|�ݭn
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
	if (type) return false;	//�T�{ Address_Level_1 �T��O�@�� Address  
	FNameParser.GetFNameStringByID(type, Type, true);
	if (Type.empty()) return false;		//���]�u�O�Φ��W�� Address�A���G�Ӱ϶��S��n�O���İO����϶��A���u�n���O���T��m���M�|Ū����r�� EX : 000002C400000043

	//Name
	int name = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.MemberFNameIndex);			// "None" Object �B"Member" Object �|���i�o��
	FNameParser.GetFNameStringByID(name, Name, true);
	if (Name.empty()) return (!Type.empty()) ? true : false;		// �p�G type ���Aname �o�O�Ū��A��ܷ�e�T��O member �u�O name offset �O�����A�ݭn autoConfig �h�ץ�

	return true;
}

bool ObjectManager::GetBasicInfo_2(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Class, DWORD_PTR& Outer)
{
	//Class
	Class = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Class);

	//Type
	int type = MemMgr.MemReader.ReadMem<int>(Class + UEOffset.FNameIndex);
	if (type) return false;			// �T�{ Class �T��O�@�� Address
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
	if (!GetBasicInfo_1(Address, RetObjectData.ObjectID, RetObjectData.Name, RetObjectData.Type, RetObjectData.Outer)) {			//²��ӻ� GetBasicInfo_1 �O���S�� Object �����A�@�b���p�h�|���� GetBasicInfo_2
		GetBasicInfo_2(Address, RetObjectData.ObjectID, RetObjectData.Name, RetObjectData.Type, ClassPtr, RetObjectData.Outer);
	}
	RetObjectData.Address = Address;
	if (RetObjectData.Name.empty()) RetObjectData.Name = "InvalidName";
	if (RetObjectData.Name.empty() or RetObjectData.Type.empty() or RetObjectData.Type.length() > 100) return false;		//���`��ƳB�z
	return true;
}

void ObjectManager::GetFullName(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	int ConcateOuterCnt = 0;
	int MaxConcateOuterCnt = 10;
	// �걵 Parent �M Child ���������Y�A�ñN�ѵ��G�x�s�b Child�A��ܤ@���q���h���L�Ӫ� FullPath
	// �b���o FullName ���P�ɡA�]�O�b�ѪR�Ҧ� Parent Object
	ObjectData NewObj, OldObj;
	std::string TempStr = RetObjectData.Name;
	NewObj = RetObjectData;
	while (true) {
		if (ConcateOuterCnt >= MaxConcateOuterCnt) break;
		ConcateOuterCnt++;

		OldObj = NewObj;
		if (NewObj.Outer == NULL or !TrySaveObject(NewObj.Outer, NewObj, Level - 1)) break;		//�p�G Outer != NULL�A�N�|�h�� TrySaveObject ���o NewObj�A��B�J�䤤�@�B�J���ѴN break

		//�O�Ѽ�(�`�N!!!�O���e�걵�A�ҥH�~�i�o�˼g)
		if ((OldObj.Type.find("Property") != std::string::npos || OldObj.Type.find("Function") != std::string::npos) &&
			(NewObj.Type.find("Property") == std::string::npos && NewObj.Type.find("Function") == std::string::npos))
			TempStr = NewObj.Name + ":" + TempStr;

		//�٬O Property �� Function
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
		BasicObjectDataWapper(PropertyObject, TempBasicObjectData);		//���] PropertyObject ���򥻸�T�� TempBasicObjectData
		RetObjectData.Property.push_back(TempBasicObjectData);			//��e��^����] RetObjectData �� Property �s�W TempBasicObjectData ����]

		//�� SubType 
		if (PropertyObject.Property.empty())			// ����̭��N�OSubType����
			RetObjectData.SubType.push_back(PropertyObject.Address);
		else										// ����̭��S�]�t�@�Ӫ���
			RetObjectData.SubType.push_back(PropertyObject.Property[0].Address);
		return true;
	}
	else return false;
}

void ObjectManager::GetProperty(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level)
{
	int16_t Offset;
	int16_t PropSize;
	int BitMask = 0;		//�����n�� Return Object ���ƭȳ��n��l��
	DWORD_PTR Property_0 = NULL, Property_8 = NULL, TypeObject = NULL;
	ObjectData PropertyObject;

	//Ū�� Offset�BPropsize
	Offset = MemMgr.MemReader.ReadMem<int16_t>(Address + UEOffset.Offset);
	RetObjectData.Offset = Offset;
	PropSize = MemMgr.MemReader.ReadMem<int16_t>(Address + UEOffset.PropSize);
	RetObjectData.PropSize = PropSize;

	//�ǳƬ����ƾ�
	Property_0 = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Property);
	Property_8 = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Property + ProcessInfo::ProcOffestAdd);
	TypeObject = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.TypeObject);
	BitMask = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.BitMask);

	//�b���󤤡A�p�G���H�U�o�ǯS���ܼơA�h�N�o���ܼƳ������@�� Object �A�~��@�ѪR
	// Class Type �� StructProperty�BObjectProperty�BClassProperty�BEnum�BArray
	// 1 �S Property �N�� TypeObject
	// 2 �� Property �N�� Property
	if (RetObjectData.Type.find("StructProperty") != std::string::npos or
		RetObjectData.Type.find("ObjectProperty") != std::string::npos or
		RetObjectData.Type.find("ClassProperty") != std::string::npos or
		RetObjectData.Type.find("ArrayProperty") != std::string::npos or
		RetObjectData.Type.find("EnumProperty") != std::string::npos or
		RetObjectData.Type.find("ByteProperty") != std::string::npos	// ByteProperty �i�ੵ���� EnumProperty
		)
	{
		// Is Property (Map's Member)
		if (PropertyProcess(RetObjectData, Property_8, Level)) {}
		// Is Property
		else if (PropertyProcess(RetObjectData, Property_0, Level)) {}
		// Is Type Object
		else if (PropertyProcess(RetObjectData, TypeObject, Level)) {}
	}
	// Class Type �� MapProperty
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
	// Class Type �� BoolProperty
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
		//���o Member �B MemberSize
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

	// ���G�� '/'
	size_t firstSlashPos = input.find('/');
	if (firstSlashPos == std::string::npos) return "";
	size_t secondSlashPos = input.find('/', firstSlashPos + 1);
	if (secondSlashPos == std::string::npos) return "";

	//3�Ӫ����ǫܭ��n�A������
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
				int NameIdx = MemMgr.MemReader.ReadMem<int>(RetObjectData.Outer + UEOffset.FNameIndex);			// "None" Object �B"Member" Object �|���i�o��
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

	//�T�{ Address �O�@�� Pointer
	if (MemMgr.MemReader.ReadMem<DWORD_PTR>(Address)) return false;

	//�d�ݸ� Address �O�_�w�g�B�z�L�A�Y�B�z�L�h������^���G
	if (StorageMgr.GetObjectDict(Address, TempObjectData)) { ObjData = TempObjectData; return true; }

	// Get Basic Info
	DWORD_PTR ClassPtr = NULL;		// <= ���� Object �� Class Address ����A�B�z (�]���� Level �`�ת����D)
	if (GetBasicInfo(RetObjectData, Address, Level, ClassPtr) == false) return false;
	if (!RetObjectData.Type.empty()) StorageMgr.ObjectTypeList.Set(RetObjectData.Type, RetObjectData.Type);	// �x�s Type ����
	if (RetObjectData.Name == "None") { ObjData = RetObjectData; return true; }			// �p�G�O None �h�����^�� BasicInfo�A�B���x�s�����Ӫ���
	if (RetObjectData.Name == "InvalidName") { ObjData = RetObjectData; return true; };

	// Get Full Name
	if (!SkipGetFullName and !Utils.ContainStr(RetObjectData.Type, "Property") and RetObjectData.Address != RetObjectData.Outer)
		GetFullName(RetObjectData, Address, Level);


	// �o��D�n�O�קK Overflow�A�u�n���ƥΧ��N���o�򥻸�T�����^�ǡA���x�s�A����i�ӸӪ���ɦA�~��ѪR
	if (Level >= MaxLevel) { ObjData = RetObjectData; return true; }

	//���Ĥ@��������s�ɡA�[�ְ���y�{
	{
		//�ˬd�O�_ Object ID �w�s�b�� Object Table �A�Y�s�b�h������^���G�A�_�h�x�s�i Object Table (By ID)			//���x�s�D���� //�D Property ����
		if (RetObjectData.Type.find("Property") == std::string::npos) {		//���O Member
			if (StorageMgr.GetObjectDictByID(Address, TempObjectData)) { ObjData = TempObjectData; return true; }

			if (RetObjectData.ObjectID < 0xFFFFFFFF) {
				StorageMgr.ObjectDictByID.Set(RetObjectData.ObjectID, RetObjectData.Address);		//By ID
			}
		}

		//�x�s�Ҧ�����A��������
		if (StorageMgr.GetObjectDict(Address, TempObjectData)) { ObjData = TempObjectData; return true; }
		else StorageMgr.ObjectDict.Set(Address, RetObjectData);		//By Address

		//�x�s�Ҧ����� (Name)
		std::string FullName = Utils.rStringToLower(RetObjectData.FullName);
		if (!StorageMgr.GetObjectDictByName(RetObjectData.Name, TempObjectData)) StorageMgr.ObjectDictByName.Set(RetObjectData.Name, RetObjectData.Address);	//By FullName
		if (!StorageMgr.GetObjectDictByFullName(FullName, TempObjectData)) StorageMgr.ObjectDictByFullName.Set(FullName, RetObjectData.Address);	//By FullName
	}

	// �p�G�O�j�M�Ҧ��A�h���o�򥻸�T�N��^
	if (SearchMode)
		return true;

	//Package Name �B�z
	SetPackage(RetObjectData);

	StorageMgr.GUObjectArrayTotalObjectCounter.Set(StorageMgr.GUObjectArrayTotalObjectCounter.Get() + 1);		// Object Counter Add 1
	//--��o�䬰��C�� ObjectData ���򥻤@�w�|�����ݩʳB�z�n�F
	//-- ���� List / Table ���H�U 3 ��
	//-- ObjectTable        Object ID
	//-- ObjectLists        Object Pointer
	//-- FullNameList       Object FullName

	//===============================================================================================================
	// �p�G�s�b Class ����A�h�~��B�z
	ObjectData ClassObject;
	if (ClassPtr != NULL) {
		if (TrySaveObject(ClassPtr, ClassObject, Level - 1)) {
			// �o�䪺 class.Instance �N�O������ӵ{���������� Object �|�ϥΨ�� class
			BasicObjectDataWapper(ClassObject, TempBasicObjectData);
			RetObjectData.ClassPtr = TempBasicObjectData;			//��a�}�� Pointer
		}
	}

	//	-- �`�N!!!!�d�M�� super �B Outer �U�O�ƻ�
	//	-- super �O�� Object �����O / ���A�A���׬O������class�άO�~�Ӫ�class
	//	-- Outer �O�� Object ���� Object�A�Ӥ� Object �� Object �O��Ӥ��P���F��A�u�O ��Object ���ϥ� Object �Ӥw�AObject �èS���ϥ� ��Object ������ѼơA�]�S�����~��
	DWORD_PTR Super;
	ObjectData SuperObject;
	if (UEOffset.UEOffestIsExist["Super"]) {		//�p�G�]�� 0 �N�����\��
		Super = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.Super);
		if (TrySaveObject(Super, SuperObject, Level - 1)) {
			// �� Uper �h���������� Object �|�ϥΨ�� �����O(Super)
			BasicObjectDataWapper(SuperObject, TempBasicObjectData);
			RetObjectData.SuperPtr = TempBasicObjectData;		//RetObjectData ���� SuperObject
		}
	}

	//�O Property Object
	if (RetObjectData.Type.find("Property") != std::string::npos) {
		// Offset �s�b�~����A�ҥH�]�� 0 �����������ﶵ
		//���o Offset�BPropsize�B(Property or BitMask)
		if (UEOffset.UEOffestIsExist["Offset"] and UEOffset.UEOffestIsExist["Property"] and UEOffset.UEOffestIsExist["PropSize"] and UEOffset.UEOffestIsExist["BitMask"])
			GetProperty(RetObjectData, Address, Level);

	}
	// ��l�� Object �����եh���o Member�A�p�G������
	else {
		// ���o Member�BMemberSize 
		if (UEOffset.UEOffestIsExist["Member"] and UEOffset.UEOffestIsExist["NextMember"])
			GetMember(RetObjectData, Address, Level);
	}



	//�O Function
	//�p�G�J�� Function ����A�h�ʺA��M function �� Offset
	if (RetObjectData.Type.find("Function") != std::string::npos) {
		if (UEOffset.UEOffestIsExist["Funct"])
			GetFunction(RetObjectData, Address, Level);
	}

	// �Ĥ@���i�Ӫ� Obejct �@�w�|���㨫�L�@��~�| Return True
	StorageMgr.ObjectDict.Set(Address, RetObjectData);		//By Address  //�o�ɭ��x�s���N�O����Ӹ�T

	//���\ !! �̲׵��G�^��
	//if (RetObjectData.FullName.find("Engine.") != std::string::npos) printf("[ FName FullName ] %p %s\n", RetObjectData.Address, RetObjectData.FullName.c_str());
	ObjData = RetObjectData;
	return true;
}