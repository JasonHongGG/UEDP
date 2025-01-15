#include "AutoConfig.h"


void UEOffsetAutoConfig::FindBasicInfoOffset()
{
    //�򥻸�T
    bool FindOuterOffset_Flag = false;
    bool FindClassOffset_Flag = false;
    DWORD_PTR GUObjectAddress = StorageMgr.GUObjectArrayBaseAddress.Get();
    size_t GUObjectArrayElementSize = StorageMgr.GUObjectArrayElementSize.Get();

    DWORD_PTR ObjectArrayEntry = NULL, ObjectEntry = NULL, SubObjectEntry = NULL, ObjectNameAddress = NULL, ObjectIDAddress;
  
    if (MemMgr.MemReader.ReadMultiLevelPointer(GUObjectAddress, 2)) {  //�ƫe�ǳ� : �Ĥ@�� Object �n�i�h��h�A�C�ӹC���i�ण�P
        ObjectArrayEntry = TempReadMemResult.first;
        for (int i = 0; i <= 10; i++) {
            MemMgr.MemReader.ReadMem(ObjectEntry, ObjectArrayEntry + i * GUObjectArrayElementSize);

            // ���W�� Object ������ 
            ObjectNameAddress = DumperUtils.CheckValue(ObjectEntry + ProcessInfo::ProcOffestAdd, 0x80, std::string("Object"), 1, true);        //�@�� Object �j�p�ܤ֦b Member ���T�w���O 0x80 //���W�� Object ������
            if (ObjectNameAddress != NULL) {
                // FName
                UEOffset.FNameIndex = ObjectNameAddress - ObjectEntry;
                UEOffset.UEOffestIsExist["FNameIndex"] = true;

                // Object ID
                ObjectIDAddress = DumperUtils.CheckValue(ObjectEntry + ProcessInfo::ProcOffestAdd, 0x80, i, 4);      // i ��ܪ����O��e Array ���� i ��
                UEOffset.ID = ObjectIDAddress - ObjectEntry;
                UEOffset.UEOffestIsExist["ObjectID"] = true;

                // �O Pointer �h�P�_�O�_�O����A�p�O����h�P�_ Name �O�_�ŦX��M�ؼЯS�x
                for (int j = 0x8; j <= 0x80; j += (int)ProcessInfo::ProcOffestAdd) {
                     MemMgr.MemReader.ReadMem(SubObjectEntry, ObjectEntry + j);
                    if (MemMgr.MemReader.IsPointer(SubObjectEntry)) {    //IsPointer
                         MemMgr.MemReader.ReadMem<int64_t>(TempFNameID, SubObjectEntry + UEOffset.FNameIndex);
                        if (FNameParser.GetFNameStringByID<int64_t>(TempFNameID, TempFNameStr, true)) {
                            // Outer
                            if (TempFNameStr.find("Core") != std::string::npos) {
                                UEOffset.Outer = j;
                                UEOffset.UEOffestIsExist["Outer"] = true;
                                FindOuterOffset_Flag = true;
                            }
                            // Class
                            else if (TempFNameStr.find("Class") != std::string::npos) {
                                UEOffset.Class = j;
                                UEOffset.UEOffestIsExist["Class"] = true;
                                FindClassOffset_Flag = true;
                            }
                        }
                    }

                    if (FindOuterOffset_Flag and FindClassOffset_Flag) break;  //��ӳ����N�i�H�����j��
                }
            }

            if (FindOuterOffset_Flag and FindClassOffset_Flag) break;
        }
    }

}

void UEOffsetAutoConfig::FindSuperAndMemberOffset(DWORD_PTR GameEngineObjectAddress, DWORD_PTR& EngineORActorObjectAddress)
{
    // EngineORActorObjectAddress �|�ھ� super ���X����������
    // => �p�G�O GameEngine �|���X Engine
    // => �p�G�O Pawn �|���X Actor

    bool FindTargetSuper_Flag = false;
    bool FindTargetNextMember_Flag = false;
    int CheckLevel = 2;
    DWORD_PTR SubObjectEntry = NULL, MemberEntry = NULL, ObjectIDAddress = NULL;

    //�ʺA��� Super�BMember�BNextMember
    for (int i = (int)UEOffset.Outer + (int)ProcessInfo::ProcOffestAdd; i < 0x100; i += (int)ProcessInfo::ProcOffestAdd) {
         MemMgr.MemReader.ReadMem(SubObjectEntry, GameEngineObjectAddress + i);

        if (ObjectMgr.TrySaveObject(SubObjectEntry, TempObj, ObjectMgr.MaxLevel)) {

            //���� 1      ��� Super
            if (!FindTargetSuper_Flag and (TempObj.FullName.find("Engine.Engine") != std::string::npos or TempObj.FullName.find("Engine.Actor") != std::string::npos)) {
                UEOffset.Super = i;
                UEOffset.UEOffestIsExist["Super"] = true;
                FindTargetSuper_Flag = true;
                EngineORActorObjectAddress = SubObjectEntry;
            }

            //���� 2      ��� Member�BNextMember
            // �X�ݸ� Object �����Ҧ��ѼơA��� Member ����Ҧb��m
            if ((TempObj.Type.find("Property") != std::string::npos or
                TempObj.Type.find("Function") != std::string::npos or
                TempObj.Type.find("Enum") != std::string::npos)
                and TempObj.FullName.find("Core") == std::string::npos) {

                for (int j = (int)UEOffset.FNameIndex; j < 0x100; j += (int)ProcessInfo::ProcOffestAdd) { // �@�w�b UEOffset.FNameIndex ����
                    if (MemMgr.MemReader.ReadMem(MemberEntry, SubObjectEntry + j)) {

                        // ��� NextMember ����
                        // �p SubObjectEntry �O Member �h��b�Y j Offset���U�A�i�H�s��X�ݪ���U�h CheckLevel ��
                        for (int k = 1; k <= CheckLevel; k++) {
                            if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
                                // �O Core ����h���ŦX
                                if ((TempObj.Type.find("Property") == std::string::npos and TempObj.Type.find("Function") == std::string::npos and
                                    TempObj.Type.find("ScriptStruct") == std::string::npos and TempObj.Type.find("State") == std::string::npos) or
                                    TempObj.FullName.find("Core") != std::string::npos) {
                                    break;
                                }

                                // ��� MemberFNameIndex Offset �p�G���s�b
                                if (!UEOffset.UEOffestIsExist["MemberFNameIndex"]) {
                                    // ���j NextMember ���󤺪��Ѽ�
                                    for (int n = (int)UEOffset.FNameIndex + (int)ProcessInfo::ProcOffestAdd; n <= 0x50; n += (int)ProcessInfo::ProcOffestAdd) {
                                        MemMgr.MemReader.ReadMem(TempFNameID, MemberEntry + n);
                                        if (FNameParser.GetFNameStringByID<int64_t>(TempFNameID, TempFNameStr, true)) { // �Φ� : 00000000(4bytes) FNameID(4bytes)
                                            if (MemMgr.MemReader.IsEqual(MemberEntry + n + 4, 0)) {
                                                UEOffset.MemberFNameIndex = n;
                                                UEOffset.UEOffestIsExist["MemberFNameIndex"] = true;
                                                break;
                                            }
                                        }
                                    }
                                }

                                //�P�_�Ӫ��� Name �ѼƦs�b
                                MemMgr.MemReader.ReadMem<int64_t>(TempFNameID, MemberEntry + UEOffset.MemberFNameIndex);
                                FNameParser.GetFNameStringByID<int64_t>(TempFNameID, TempFNameStr, true);
                                if (TempFNameStr.empty()) break;

                                // �O�_�w�s��ŦX���� CheckLevel ��
                                if (k == CheckLevel) {
                                    UEOffset.NextMember = j;
                                    UEOffset.UEOffestIsExist["NextMember"] = true;
                                    UEOffset.Member = i;
                                    UEOffset.UEOffestIsExist["Member"] = true;
                                    FindTargetNextMember_Flag = true;
                                    break;
                                }

                                // �U�@�h
                                if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + j)) break; //�S���U�@�h��ܤ��ŦX
                            }
                            else break;
                        }
                    }
                    if (FindTargetNextMember_Flag) break;
                }
            }

            // Break
            if (FindTargetSuper_Flag and FindTargetNextMember_Flag) break;
        }

    }
}

void UEOffsetAutoConfig::FindProperty(DWORD_PTR EngineORActorObjectAddress)
{
    // �ʺA��� UObject.propsize     (�ھڤw�������A�j�p => int 4bytes�Bfloat 4 bytes ...�A�ھ� Object Type �b����O����ϧ֧�M�S�w�� 2bytes �ƭ�)
    // �H Engine�BActor �h��    // �̦h�X�� 300 �� member
    bool FindTargetObject_Flag = false;
    DWORD_PTR MemberEntry = NULL;
    MemMgr.MemReader.ReadMem(MemberEntry, EngineORActorObjectAddress + UEOffset.Member);
    for (int i = 0; i <= 300; i++) {
        if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
            // ObjectProperty Type �O 8�A�b�O���餤���� Address ����ַ|�� "08" �o�� 2 Bytes�A ��L���O ClassProperty �]�i�H�� 8 bytes
            if (TempObj.Type.find("ObjectProperty") != std::string::npos) {
                FindTargetObject_Flag = true;
                break;
            }
        }
        if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + UEOffset.NextMember)) break;
    }
    if (!FindTargetObject_Flag) return;

    // ��� PropSizeAddress
    DWORD_PTR PropSizeAddress = DumperUtils.CheckValue(MemberEntry + UEOffset.Outer + ProcessInfo::ProcOffestAdd, 0x100, UETypeTable[TempObj.Type], 2);
    UEOffset.PropSize = PropSizeAddress - MemberEntry;
    UEOffset.UEOffestIsExist["PropSize"] = true;

}

void UEOffsetAutoConfig::FindOffsetOffset(DWORD_PTR EngineORActorObjectAddress)
{
    // �ʺA��� UObject.Offset
    // �̦h�X�� 300 �� member
    bool FindTargetObject_Flag = false;
    DWORD_PTR MemberEntry = NULL;
    DWORD_PTR NextMemberEntry = NULL;
    MemMgr.MemReader.ReadMem(MemberEntry, EngineORActorObjectAddress + UEOffset.Member);
    for (int i = 0; i <= 300; i++) {
        if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
            if (TempObj.Type.find("ObjectProperty") != std::string::npos or
                TempObj.Type.find("ClassProperty") != std::string::npos or
                TempObj.Type.find("FloatProperty") != std::string::npos or
                TempObj.Type.find("IntProperty") != std::string::npos)
            {
                FindTargetObject_Flag = true;
                break;
            }
        }
        if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + UEOffset.NextMember)) break;
    }
    if (!FindTargetObject_Flag) return;

    // �M�� MemberEntry + i + varsize �M NextMemberEntry + i �� 2bytes �ƭȬۦP�� i Offset
    // ��ӾF�� Member �����P�˰��� i�AŪ�� 2 bytes ���ƭȡA�ӼƭȪ��t�Z�|�O NextMemberEntry �� Type Size
    // EX: MemberEntry = 0x10�AMemberEntry �O int ���A�A�ҥH Address_Level_3 �N�|�O 0x14     (�t�Z�O Address_Level_2 �� Type Size)
    int16_t TempInt_1 = 0, TempInt_2 = 0;
    if (!MemMgr.MemReader.ReadMem(NextMemberEntry, MemberEntry + UEOffset.NextMember)) return;
    for (int i = (int)UEOffset.NextMember + (int)ProcessInfo::ProcOffestAdd; i <= 0x100; i += 0x2) {
        MemMgr.MemReader.ReadMem(TempInt_1, MemberEntry + i);
        MemMgr.MemReader.ReadMem(TempInt_2, NextMemberEntry + i);

        if (TempInt_1 + UETypeTable[TempObj.Type] >= 0x20 /*�]�w�̤p�ƭȡA�çK�~�P*/ and TempInt_1 + UETypeTable[TempObj.Type] == TempInt_2) {
            UEOffset.Offset = i;
            UEOffset.UEOffestIsExist["Offset"] = true;
            break;
        }
    }
}

void UEOffsetAutoConfig::FindBitMaskOffset()
{
    // �ʺA��� UObject.bitmask
    // �̦h�X�� 300 �� member
    ObjectData ActorObject;
    bool FindTargetObject_Flag = false;
    DWORD_PTR MemberEntry = NULL;
    DWORD_PTR NextMemberEntry = NULL;
    int16_t TempInt_1 = 0, TempInt_2 = 0;

    // ���e���� BoolProperty �O�P�@�ժ� Member
    if (StorageMgr.GetObjectDictByName("Actor", TempObj)) ActorObject = TempObj;
    MemMgr.MemReader.ReadMem(MemberEntry, ActorObject.Address + UEOffset.Member);
    for (int i = 0; i <= 300; i++) {
        if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel) and TempObj.Type.find("BoolProperty") != std::string::npos) {
            if (MemMgr.MemReader.ReadMem(NextMemberEntry, MemberEntry + UEOffset.NextMember)) {
                MemMgr.MemReader.ReadMem(TempInt_1, MemberEntry + UEOffset.Offset);
                MemMgr.MemReader.ReadMem(TempInt_2, NextMemberEntry + UEOffset.Offset);

                if (TempInt_1 == TempInt_2) { FindTargetObject_Flag = true;  break; }
            }
        }
        if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + UEOffset.NextMember)) break;
    }
    if (!FindTargetObject_Flag) return;


    // BoolProperty �̦h 8 �Ӥ@�աA�B�s��ƦC�q�p��j
    // ���߱��� : �@�w�n Address_Level_1 �� Object �@�w�n�b�Ĥ@�Ӧ�m�A�]�N�O +0x??:0  ����m
    // => �p���~��O�� TempByte_1 = 1�BTempByte_2 �O 2 ������
    // => :0 (01) :1 (10) :2(100{2}) :2(1000{3})    //�i�H�ݨ�O 1 �Ҧb byte ����m�|ı�o�L�� offset�A�o�N�O mask ����z
    if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
        BYTE TempByte_1 = 0, TempByte_2 = 0;
        for (int i = (int)0x70; i <= 0x100; i++) {

             MemMgr.MemReader.ReadMem(TempByte_1, MemberEntry + i);
             MemMgr.MemReader.ReadMem(TempByte_2, NextMemberEntry + i);

            if ((TempByte_1 == 1 and TempByte_2 % 2 == 0) and (TempByte_1 < TempByte_2)) {
                UEOffset.BitMask = i;
                UEOffset.UEOffestIsExist["BitMask"] = true;
                break;
            }
        }
    }

}

void UEOffsetAutoConfig::AutoConfig() {
    // Check Require
    if (!StorageMgr.FNamePoolBaseAddress.IsInitialized()) {
        printf("[ FNamePool Not Exist ]\n");
        return;
    }
    if (!StorageMgr.GUObjectArrayBaseAddress.IsInitialized()) {
        printf("[ GUObjectArray Not Exist ]\n");
        return;
    }
    if (StorageMgr.FNamePoolParseExecutedFlag.Get()) {
        printf("[ Please Execute ParseFNamePool First ]\n\n");
        return;
    }


    // �����X GameEngine �� Pawn ����
    ObjectData GameEngineObject, ActorObject;
    DWORD_PTR EngineORActorObjectAddress = NULL;
    DWORD_PTR GameEngineObjectAddress = GUObjectParser.FindObjectByFullName("/Script/Engine.GameEngine");      //Engine.GameEngine�BEngine.Pawn
    if (GameEngineObjectAddress == NULL) {
        GameEngineObjectAddress = GUObjectParser.FindObjectByFullName("/Script/Engine.Pawn");
        if (GameEngineObjectAddress == NULL)
            if (StorageMgr.GetObjectDictByName("GameEngine", TempObj) or StorageMgr.GetObjectDictByName("Pawn", TempObj)) GameEngineObjectAddress = TempObj.Address;   //GameEngine�BPawn
    }

    FindBasicInfoOffset();
    if (ObjectMgr.TrySaveObject(GameEngineObjectAddress, GameEngineObject, ObjectMgr.MaxLevel))
    {
        FindSuperAndMemberOffset(GameEngineObjectAddress, EngineORActorObjectAddress);
        FindProperty(EngineORActorObjectAddress);
        FindOffsetOffset(EngineORActorObjectAddress);
        FindBitMaskOffset();

        printf("\n[[ \n"
            "[UObject]\n"
            "UObject.ObjectId = 0x%X\n"
            "UObject.Class = 0x%X\n"
            "UObject.FNameIndex = 0x%X\n"
            "UObject.Outer = 0x%X\n"
            "UObject.super = 0x%X\n"
            "UObject.member = 0x%X\n"
            "UObject.nextmember = 0x%X\n"
            "UObject.MemberFNameIndex = 0x%X\n"
            "UObject.Offset = 0x%X\n"
            "UObject.propsize = 0x%X\n"
            "UObject.Property = 0x%X\n"
            "UObject.bitmask = 0x%X\n"
            "]]\n\n"
            , (int)UEOffset.ID, (int)UEOffset.Class, (int)UEOffset.FNameIndex, (int)UEOffset.Outer, (int)UEOffset.Super
            , (int)UEOffset.Member, (int)UEOffset.NextMember, (int)UEOffset.MemberFNameIndex, (int)UEOffset.Offset, (int)UEOffset.PropSize, (int)UEOffset.Property, (int)UEOffset.BitMask);
    }
}
