#include "AutoConfig.h"


void UEOffsetAutoConfig::FindBasicInfoOffset()
{
    //基本資訊
    bool FindOuterOffset_Flag = false;
    bool FindClassOffset_Flag = false;
    DWORD_PTR GUObjectAddress = StorageMgr.GUObjectArrayBaseAddress.Get();
    size_t GUObjectArrayElementSize = StorageMgr.GUObjectArrayElementSize.Get();

    DWORD_PTR ObjectArrayEntry = NULL, ObjectEntry = NULL, SubObjectEntry = NULL, ObjectNameAddress = NULL, ObjectIDAddress;
  
    if (MemMgr.MemReader.ReadMultiLevelPointer(GUObjectAddress, 2)) {  //事前準備 : 第一個 Object 要進去兩層，每個遊戲可能不同
        ObjectArrayEntry = TempReadMemResult.first;
        for (int i = 0; i <= 10; i++) {
            MemMgr.MemReader.ReadMem(ObjectEntry, ObjectArrayEntry + i * GUObjectArrayElementSize);

            // 找到名為 Object 的物件 
            ObjectNameAddress = DumperUtils.CheckValue(ObjectEntry + ProcessInfo::ProcOffestAdd, 0x80, std::string("Object"), 1, true);        //一個 Object 大小至少在 Member 中固定都是 0x80 //找到名為 Object 的物件
            if (ObjectNameAddress != NULL) {
                // FName
                UEOffset.FNameIndex = ObjectNameAddress - ObjectEntry;
                UEOffset.UEOffestIsExist["FNameIndex"] = true;

                // Object ID
                ObjectIDAddress = DumperUtils.CheckValue(ObjectEntry + ProcessInfo::ProcOffestAdd, 0x80, i, 4);      // i 表示的正是當前 Array 的第 i 項
                UEOffset.ID = ObjectIDAddress - ObjectEntry;
                UEOffset.UEOffestIsExist["ObjectID"] = true;

                // 是 Pointer 則判斷是否是物件，如是物件則判斷 Name 是否符合找尋目標特徵
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

                    if (FindOuterOffset_Flag and FindClassOffset_Flag) break;  //兩個都找到就可以結束迴圈
                }
            }

            if (FindOuterOffset_Flag and FindClassOffset_Flag) break;
        }
    }

}

void UEOffsetAutoConfig::FindSuperAndMemberOffset(DWORD_PTR GameEngineObjectAddress, DWORD_PTR& EngineORActorObjectAddress)
{
    // EngineORActorObjectAddress 會根據 super 取出對應的物件
    // => 如果是 GameEngine 會取出 Engine
    // => 如果是 Pawn 會取出 Actor

    bool FindTargetSuper_Flag = false;
    bool FindTargetNextMember_Flag = false;
    int CheckLevel = 2;
    DWORD_PTR SubObjectEntry = NULL, MemberEntry = NULL, ObjectIDAddress = NULL;

    //動態找到 Super、Member、NextMember
    for (int i = (int)UEOffset.Outer + (int)ProcessInfo::ProcOffestAdd; i < 0x100; i += (int)ProcessInfo::ProcOffestAdd) {
         MemMgr.MemReader.ReadMem(SubObjectEntry, GameEngineObjectAddress + i);

        if (ObjectMgr.TrySaveObject(SubObjectEntry, TempObj, ObjectMgr.MaxLevel)) {

            //條件 1      找到 Super
            if (!FindTargetSuper_Flag and (TempObj.FullName.find("Engine.Engine") != std::string::npos or TempObj.FullName.find("Engine.Actor") != std::string::npos)) {
                UEOffset.Super = i;
                UEOffset.UEOffestIsExist["Super"] = true;
                FindTargetSuper_Flag = true;
                EngineORActorObjectAddress = SubObjectEntry;
            }

            //條件 2      找到 Member、NextMember
            // 訪問該 Object 內的所有參數，找到 Member 物件所在位置
            if ((TempObj.Type.find("Property") != std::string::npos or
                TempObj.Type.find("Function") != std::string::npos or
                TempObj.Type.find("Enum") != std::string::npos)
                and TempObj.FullName.find("Core") == std::string::npos) {

                for (int j = (int)UEOffset.FNameIndex; j < 0x100; j += (int)ProcessInfo::ProcOffestAdd) { // 一定在 UEOffset.FNameIndex 之後
                    if (MemMgr.MemReader.ReadMem(MemberEntry, SubObjectEntry + j)) {

                        // 找到 NextMember 物件
                        // 如 SubObjectEntry 是 Member 則找在某 j Offset之下，可以連續訪問物件下去 CheckLevel 次
                        for (int k = 1; k <= CheckLevel; k++) {
                            if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
                                // 是 Core 物件則不符合
                                if ((TempObj.Type.find("Property") == std::string::npos and TempObj.Type.find("Function") == std::string::npos and
                                    TempObj.Type.find("ScriptStruct") == std::string::npos and TempObj.Type.find("State") == std::string::npos) or
                                    TempObj.FullName.find("Core") != std::string::npos) {
                                    break;
                                }

                                // 找到 MemberFNameIndex Offset 如果不存在
                                if (!UEOffset.UEOffestIsExist["MemberFNameIndex"]) {
                                    // 遞迴 NextMember 物件內的參數
                                    for (int n = (int)UEOffset.FNameIndex + (int)ProcessInfo::ProcOffestAdd; n <= 0x50; n += (int)ProcessInfo::ProcOffestAdd) {
                                        MemMgr.MemReader.ReadMem(TempFNameID, MemberEntry + n);
                                        if (FNameParser.GetFNameStringByID<int64_t>(TempFNameID, TempFNameStr, true)) { // 形式 : 00000000(4bytes) FNameID(4bytes)
                                            if (MemMgr.MemReader.IsEqual(MemberEntry + n + 4, 0)) {
                                                UEOffset.MemberFNameIndex = n;
                                                UEOffset.UEOffestIsExist["MemberFNameIndex"] = true;
                                                break;
                                            }
                                        }
                                    }
                                }

                                //判斷該物件 Name 參數存在
                                MemMgr.MemReader.ReadMem<int64_t>(TempFNameID, MemberEntry + UEOffset.MemberFNameIndex);
                                FNameParser.GetFNameStringByID<int64_t>(TempFNameID, TempFNameStr, true);
                                if (TempFNameStr.empty()) break;

                                // 是否已連續符合條件 CheckLevel 次
                                if (k == CheckLevel) {
                                    UEOffset.NextMember = j;
                                    UEOffset.UEOffestIsExist["NextMember"] = true;
                                    UEOffset.Member = i;
                                    UEOffset.UEOffestIsExist["Member"] = true;
                                    FindTargetNextMember_Flag = true;
                                    break;
                                }

                                // 下一層
                                if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + j)) break; //沒有下一層表示不符合
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
    // 動態找到 UObject.propsize     (根據已知的型態大小 => int 4bytes、float 4 bytes ...，根據 Object Type 在附近記憶體區快找尋特定的 2bytes 數值)
    // 以 Engine、Actor 去找    // 最多訪問 300 個 member
    bool FindTargetObject_Flag = false;
    DWORD_PTR MemberEntry = NULL;
    MemMgr.MemReader.ReadMem(MemberEntry, EngineORActorObjectAddress + UEOffset.Member);
    for (int i = 0; i <= 300; i++) {
        if (ObjectMgr.TrySaveObject(MemberEntry, TempObj, ObjectMgr.MaxLevel)) {
            // ObjectProperty Type 是 8，在記憶體中物件 Address 比較少會有 "08" 這樣 2 Bytes， 其他像是 ClassProperty 也可以都 8 bytes
            if (TempObj.Type.find("ObjectProperty") != std::string::npos) {
                FindTargetObject_Flag = true;
                break;
            }
        }
        if (!MemMgr.MemReader.ReadMem(MemberEntry, MemberEntry + UEOffset.NextMember)) break;
    }
    if (!FindTargetObject_Flag) return;

    // 找到 PropSizeAddress
    DWORD_PTR PropSizeAddress = DumperUtils.CheckValue(MemberEntry + UEOffset.Outer + ProcessInfo::ProcOffestAdd, 0x100, UETypeTable[TempObj.Type], 2);
    UEOffset.PropSize = PropSizeAddress - MemberEntry;
    UEOffset.UEOffestIsExist["PropSize"] = true;

}

void UEOffsetAutoConfig::FindOffsetOffset(DWORD_PTR EngineORActorObjectAddress)
{
    // 動態找到 UObject.Offset
    // 最多訪問 300 個 member
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

    // 尋找 MemberEntry + i + varsize 和 NextMemberEntry + i 的 2bytes 數值相同的 i Offset
    // 兩個鄰近 Member 之間同樣偏移 i，讀取 2 bytes 的數值，該數值的差距會是 NextMemberEntry 的 Type Size
    // EX: MemberEntry = 0x10，MemberEntry 是 int 型態，所以 Address_Level_3 就會是 0x14     (差距是 Address_Level_2 的 Type Size)
    int16_t TempInt_1 = 0, TempInt_2 = 0;
    if (!MemMgr.MemReader.ReadMem(NextMemberEntry, MemberEntry + UEOffset.NextMember)) return;
    for (int i = (int)UEOffset.NextMember + (int)ProcessInfo::ProcOffestAdd; i <= 0x100; i += 0x2) {
        MemMgr.MemReader.ReadMem(TempInt_1, MemberEntry + i);
        MemMgr.MemReader.ReadMem(TempInt_2, NextMemberEntry + i);

        if (TempInt_1 + UETypeTable[TempObj.Type] >= 0x20 /*設定最小數值，並免誤判*/ and TempInt_1 + UETypeTable[TempObj.Type] == TempInt_2) {
            UEOffset.Offset = i;
            UEOffset.UEOffestIsExist["Offset"] = true;
            break;
        }
    }
}

void UEOffsetAutoConfig::FindBitMaskOffset()
{
    // 動態找到 UObject.bitmask
    // 最多訪問 300 個 member
    ObjectData ActorObject;
    bool FindTargetObject_Flag = false;
    DWORD_PTR MemberEntry = NULL;
    DWORD_PTR NextMemberEntry = NULL;
    int16_t TempInt_1 = 0, TempInt_2 = 0;

    // 找到前後兩個 BoolProperty 是同一組的 Member
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


    // BoolProperty 最多 8 個一組，且連續排列從小到大
    // 成立條件 : 一定要 Address_Level_1 的 Object 一定要在第一個位置，也就是 +0x??:0  的位置
    // => 如此才能保證 TempByte_1 = 1、TempByte_2 是 2 的倍數
    // => :0 (01) :1 (10) :2(100{2}) :2(1000{3})    //可以看到是 1 所在 byte 的位置會覺得他的 offset，這就是 mask 的原理
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


    // 先取出 GameEngine 或 Pawn 物件
    ObjectData GameEngineObject, ActorObject;
    DWORD_PTR EngineORActorObjectAddress = NULL;
    DWORD_PTR GameEngineObjectAddress = GUObjectParser.FindObjectByFullName("/Script/Engine.GameEngine");      //Engine.GameEngine、Engine.Pawn
    if (GameEngineObjectAddress == NULL) {
        GameEngineObjectAddress = GUObjectParser.FindObjectByFullName("/Script/Engine.Pawn");
        if (GameEngineObjectAddress == NULL)
            if (StorageMgr.GetObjectDictByName("GameEngine", TempObj) or StorageMgr.GetObjectDictByName("Pawn", TempObj)) GameEngineObjectAddress = TempObj.Address;   //GameEngine、Pawn
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
