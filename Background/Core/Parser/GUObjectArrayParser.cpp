#include "GUObjectArrayParser.h"


void GUObjectArrayParserClass::Thread_SearchAllObject(DWORD_PTR Address, size_t Start, size_t End, size_t GUObjectArray_ArrEleSize, std::string FullName) {
    DWORD_PTR Address_Level_2 = NULL;
    DWORD_PTR Address_Level_3 = NULL;
    ObjectData TempObjData;
    bool SearchMode = (!FullName.empty() ? true : false);

    if (MemMgr.MemReader.ReadMem(Address_Level_2, Address)) {
        for (int i = (int)Start; i <= (int)End; i++) {
            if (MemMgr.MemReader.ReadMem(Address_Level_3, Address_Level_2 + i * GUObjectArray_ArrEleSize)) { //Object
                if (!MemMgr.MemReader.IsPointer(Address_Level_3)) break;
                ObjectMgr.TrySaveObject(Address_Level_3, TempObjData, ObjectMgr.MaxLevel, false, SearchMode);

                // �פ����
                if (SearchMode) {
                    if (StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get() != NULL) return;
                    else if (StorageMgr.GetObjectDictByFullName(FullName, TempObjData)) {    //�����A��ܳo�@���� Address �N�O�ؼ� Object
                        StorageMgr.GUObjectArrayFindObjectAddressByFullName.Set(TempObjData.Address);
                        return;  //��� => ��������
                    }
                }
                else if (StorageMgr.GUObjectArrayParseIndex.Get() > MaxObjectArray or StorageMgr.GUObjectArrayTotalObjectCounter.Get() > MaxObjectQuantity) return;
            }
        }
    }
}

DWORD_PTR GUObjectArrayParserClass::ParseGUObjectArray(GUObjectExecuteMode Mode, std::string FullName) {
    if (!StorageMgr.FNamePoolBaseAddress.IsInitialized()) {
        printf("[ FNamePool Not Exist ]\n");
        return NULL;
    }
    if (!StorageMgr.GUObjectArrayBaseAddress.IsInitialized()) {
        printf("[ GUObjectArray Not Exist ]\n");
        return NULL;
    }
    if (!StorageMgr.FNamePoolParseExecutedFlag.Get()) {
        printf("[ Please Execute ParseFNamePool First ]\n\n");
        return NULL;
    }

    if (Mode == GUObjectExecuteMode::Search) {
        //�ഫ���p�g
        if (!FullName.empty())  Utils.StringToLower(FullName);

        //�p�G�w�g�B�z�L�A�N�����^�ǵ��G
        ObjectData TempObjectData;
        if (StorageMgr.GetObjectDictByFullName(FullName, TempObjectData)) return TempObjectData.Address;
    }

    //��l�Ƭ����ƾ�
    StorageMgr.ObjectDict.Clear();
    StorageMgr.ObjectDictByID.Clear();
    StorageMgr.ObjectDictByAddress.Clear();
    StorageMgr.GUObjectArrayTotalObjectCounter.Set(0);

    //�Ы� Thread Pool
    BS::thread_pool Pool;  //BS::thread_pool pool(6) �]�m�w��֤߼�
    printf("[ Get Thread Count ] %d \n", Pool.get_thread_count());

    //�D�{���}�l�A���j GUObjectArray ���ؼ� Object
    size_t LoopStep = ProcessInfo::ProcOffestAdd;
    DWORD_PTR Address_Level_1 = NULL;
    size_t TempGUObjectArraySize;

    DWORD_PTR GUObjectArrayBaseAddress = StorageMgr.GUObjectArrayBaseAddress.Get();
    size_t GUObjectArrayElementCnt = 0x20;
    size_t GUObjectArrayElementSize = StorageMgr.GUObjectArrayElementSize.Get();
    size_t GUObjectArrayBatchSize = GUObjectArrayElementSize * GUObjectArrayElementCnt;        //�� 0x200 �� GUObject ���@�հ����j
    size_t SplitGUObjectArraySize = 0;
    for (int i = 0x0; i < 0x1000; i += (int)LoopStep) {        //0x1000          //�]�\�o���u 0x1000�A�ӬO�ھڥӽЪ��O����Ŷ��j�p�M�w
        if (MemMgr.MemReader.ReadMem(Address_Level_1, GUObjectArrayBaseAddress + i)) {

            //Address_Level_1 Is not Pointer
            if (!MemMgr.MemReader.IsPointer(Address_Level_1)) continue;
            else {
                if (MemMgr.RegionEnumerator.GetMemoryRegionSizeByAddress(ProcessInfo::hProcess, Address_Level_1, TempGUObjectArraySize, false)) {
                    SplitGUObjectArraySize = floor((TempGUObjectArraySize / GUObjectArrayBatchSize) + 0.5);
                    StorageMgr.GUObjectArrayParseIndex.Set(i);
                    StorageMgr.GUObjectArrayParseProgressBarValue.Set(0);
                    StorageMgr.GUObjectArrayParseProgressBarTotalValue.Set(SplitGUObjectArraySize);
                    ProgressBarState.GUObjectArrayEvent = ProcessState::Processing;
                    const BS::multi_future<void> loop_future = Pool.submit_loop<size_t>(0, SplitGUObjectArraySize,
                        [this, Address_Level_1, GUObjectArrayBatchSize, GUObjectArrayElementSize, Mode, FullName](const size_t i)
                        {
                            size_t Start = i * GUObjectArrayBatchSize;
                            size_t End = Start + GUObjectArrayBatchSize;          //Array �q 0 �}�l   //size_t End = Start + BatchNum - 1;  

                            if (Mode == GUObjectExecuteMode::Search and StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get() != NULL) return;
                            else if (StorageMgr.GUObjectArrayParseIndex.Get() > MaxObjectArray or StorageMgr.GUObjectArrayTotalObjectCounter.Get() > MaxObjectQuantity) return;

                            Thread_SearchAllObject(Address_Level_1, Start, End, GUObjectArrayElementSize, FullName);
                            StorageMgr.GUObjectArrayParseProgressBarValue.Set(StorageMgr.GUObjectArrayParseProgressBarValue.Get() + 1);
                        });
                    loop_future.wait();

                }
                if (Const::PrintDumpInfo) printf("[ %4d Get Region Size ] %p \t%08X\n", (int)i + 1, (void*)Address_Level_1, (int)TempGUObjectArraySize);
                if (Mode == GUObjectExecuteMode::Search and StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get() != NULL) break;
                else if (StorageMgr.GUObjectArrayParseIndex.Get() > MaxObjectArray or StorageMgr.GUObjectArrayTotalObjectCounter.Get() > MaxObjectQuantity) break;
            }
        }

    }
    ProgressBarState.GUObjectArrayEvent = ProcessState::Completed;

    //�L�X���� Object �������T��
    //for (const auto& TypeList : StorageMgr.GetObjectTypeListVector()) {
    //    printf("[ Object Type List ] %s\n", TypeList.first.c_str());
    //}
    //printf("\n");

    if (Mode == GUObjectExecuteMode::Search) {
        if (StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get() != NULL)    printf("[ Find The Object !!! ] %p\n\n", (void*)StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get());
        else                                                        printf("[ Object Not Find !!! ]\n\n");

        DWORD_PTR Result = StorageMgr.GUObjectArrayFindObjectAddressByFullName.Get();
        StorageMgr.GUObjectArrayFindObjectAddressByFullName.Set(NULL);    //��l�ơA�U���~���~��ϥ�
        return Result;      //�^�Ǫ��O�ؼ� Object ���}�Y Address
    }
    else if (Mode == GUObjectExecuteMode::Parse) {
        // �������] Package Name List
        std::vector<std::string> PackageNameList;
        for (const auto& PackageList : StorageMgr.GetPackage()) {
            PackageNameList.push_back(PackageList.first);
            if (Const::PrintDumpInfo) printf("[ Package Name List ] %s\n", PackageList.first.c_str());
        }
        StorageMgr.SetPackageDataList(PackageNameList); 
        printf("[ Package Name List Size ] %llu\n", StorageMgr.GetPackage().size());
        printf("\n");
    }
}

DWORD_PTR GUObjectArrayParserClass::FindObjectByFullName(std::string FullName) {
    return ParseGUObjectArray(GUObjectExecuteMode::Search, FullName);
}



