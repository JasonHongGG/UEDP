#include "FNamePoolParser.h"

void FNamePoolParserClass::Thread_FindString(const std::string InputString, size_t Start, size_t End) {
    std::string Name;
    for (size_t i = Start; i <= End; i++) {
        Name = "";
        GetFNameStringByID<long long int>(i, Name, true);

        //�פ����
        if (!Name.empty() && Name.length() >= 1 && Name.length() <= 200 && !Utils.IsStringContainControlChar(Name)) {
            if (Name.find(InputString) != std::string::npos) {
                StorageMgr.FNamePoolFindStringFlag.Set(true);
                return;
            }
        }
    }
}

bool FNamePoolParserClass::FindStringFromFNamePool(const std::string& InputString) {
    BS::thread_pool Pool;  //BS::thread_pool pool(6) �]�m�w��֤߼�
    printf("[ Get Thread Count ] %d \n", Pool.get_thread_count());

    // ���] UEVersion > 23
    size_t BatchNum = 0x200;
    size_t TotalFNamePoolSize = StorageMgr.FNamePoolSize.Get() << 0x10; //�� 2^16 (4bytes)
    size_t SplitFNamePoolSize = floor((TotalFNamePoolSize / BatchNum) + 0.5);  //�N��� FNamePool �q���h�ӳ����h�B�z => �ϥ� thread �[�ֳB�z�t��

    //ProgressBar �ǳ�
    StorageMgr.FNamePoolParseProgressBarTotalValue.Set(SplitFNamePoolSize);
    ProgressBarStateMgr.FNamePool = ProcessState::Processing;

    const BS::multi_future<void> loop_future = Pool.submit_loop<size_t>(0, SplitFNamePoolSize,
        [this, InputString, BatchNum](const size_t i)
        {
            size_t Start = i * BatchNum;
            size_t End = Start + BatchNum - 1;

            if (StorageMgr.FNamePoolFindStringFlag.Get()) return;
            Thread_FindString(InputString, Start, End);

            StorageMgr.FNamePoolParseProgressBarValue.Set(StorageMgr.FNamePoolParseProgressBarValue.Get() + 1); //�аO�����[�@
        });
    loop_future.wait();
    ProgressBarStateMgr.FNamePool = ProcessState::Completed;

    if (StorageMgr.FNamePoolFindStringFlag.Get()) {
        StorageMgr.FNamePoolFindStringFlag.Set(false);
        return true;
    }
    else return false;
}

void FNamePoolParserClass::ParseFNamePool() {
    if (StorageMgr.FNamePoolBaseAddress.IsInitialized()) {
        printf("[ FNamePool Not Exist ]\n");
        printf("[ Please Get The FNamePool First ]\n\n");
        return;
    }
    StorageMgr.FNamePoolParseExecutedFlag.Set(true);
    DWORD_PTR FNamePool = StorageMgr.FNamePoolBaseAddress.Get();
    DWORD_PTR FNamePool_Entry = FNamePool + StorageMgr.FNamePoolFirstPoolOffest.Get();

    int NullCnt = 0;        //�p��J�� Null Address ������
    int FNamePoolCnt = 0;
    DWORD_PTR Address_Level_1;

    for (int i = 0; i < 500; i++) {                 //�̦h�u�� 500 �� FNamePool => �A�h�N�߱� (�q�`������ 100 �ӡA���h 50 �X��)
        Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(FNamePool_Entry + i * (8 - ProcessInfo::ProcOffestSub));              //32bit => 4bytes �@��   //64bit => 8bytes �@��
        if (Address_Level_1) {
            if (MemMgr.MemReader.ReadMem<DWORD_PTR>(Address_Level_1)) {    // Is Pointer ?
                StorageMgr.FNamePoolSize.Get();
                FNamePoolCnt += 1;
            }
            else {
                NullCnt += 1;
            }

            if (NullCnt >= 3) break;    //�w�g�� FNamePool �����F
        }
    }

    printf("[ FNamePool Quantity ] %d\n", FNamePoolCnt);
    bool CoreUObjectFind_Flag = FindStringFromFNamePool("/Script/CoreUObject");
    if (CoreUObjectFind_Flag) printf("[ UE Version >= 4 ]\n\n");
    else printf("[ UE Version == 3 ]\n\n");

}
