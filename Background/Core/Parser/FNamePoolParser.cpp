#include "FNamePoolParser.h"

void FNamePoolParserClass::Thread_FindString(const std::string InputString, size_t Start, size_t End) {
    std::string Name;
    for (size_t i = Start; i <= End; i++) {
        Name = "";
        GetFNameStringByID<long long int>(i, Name, true);

        //終止條件
        if (!Name.empty() && Name.length() >= 1 && Name.length() <= 200 && !Utils.IsStringContainControlChar(Name)) {
            if (Name.find(InputString) != std::string::npos) {
                StorageMgr.FNamePoolFindStringFlag.Set(true);
                return;
            }
        }
    }
}

bool FNamePoolParserClass::FindStringFromFNamePool(const std::string& InputString) {
    BS::thread_pool Pool;  //BS::thread_pool pool(6) 設置硬體核心數
    printf("[ Get Thread Count ] %d \n", Pool.get_thread_count());

    // 假設 UEVersion > 23
    size_t BatchNum = 0x200;
    size_t TotalFNamePoolSize = StorageMgr.FNamePoolSize.Get() << 0x10; //乘 2^16 (4bytes)
    size_t SplitFNamePoolSize = floor((TotalFNamePoolSize / BatchNum) + 0.5);  //將整個 FNamePool 猜成多個部份去處理 => 使用 thread 加快處理速度

    //ProgressBar 準備
    StorageMgr.FNamePoolParseProgressBarTotalValue.Set(SplitFNamePoolSize);
    ProgressBarStateMgr.FNamePool = ProcessState::Processing;

    const BS::multi_future<void> loop_future = Pool.submit_loop<size_t>(0, SplitFNamePoolSize,
        [this, InputString, BatchNum](const size_t i)
        {
            size_t Start = i * BatchNum;
            size_t End = Start + BatchNum - 1;

            if (StorageMgr.FNamePoolFindStringFlag.Get()) return;
            Thread_FindString(InputString, Start, End);

            StorageMgr.FNamePoolParseProgressBarValue.Set(StorageMgr.FNamePoolParseProgressBarValue.Get() + 1); //標記完成加一
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

    int NullCnt = 0;        //計算遇到 Null Address 的次數
    int FNamePoolCnt = 0;
    DWORD_PTR Address_Level_1;

    for (int i = 0; i < 500; i++) {                 //最多只掃 500 的 FNamePool => 再多就拋棄 (通常都不到 100 個，頂多 50 幾個)
        Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(FNamePool_Entry + i * (8 - ProcessInfo::ProcOffestSub));              //32bit => 4bytes 一組   //64bit => 8bytes 一組
        if (Address_Level_1) {
            if (MemMgr.MemReader.ReadMem<DWORD_PTR>(Address_Level_1)) {    // Is Pointer ?
                StorageMgr.FNamePoolSize.Get();
                FNamePoolCnt += 1;
            }
            else {
                NullCnt += 1;
            }

            if (NullCnt >= 3) break;    //已經到 FNamePool 結束了
        }
    }

    printf("[ FNamePool Quantity ] %d\n", FNamePoolCnt);
    bool CoreUObjectFind_Flag = FindStringFromFNamePool("/Script/CoreUObject");
    if (CoreUObjectFind_Flag) printf("[ UE Version >= 4 ]\n\n");
    else printf("[ UE Version == 3 ]\n\n");

}
