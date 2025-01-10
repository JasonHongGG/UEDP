#pragma once
#include <Windows.h>
#include <string>
#include "../../../State/GUIState.h"
#include "../../Storage/StorageManager.h"
#include "../../../Utils/Include/BS_thread_pool.hpp"
#include "../../../Utils/Utils.h"

#include "../../../System/Memory.h"
#include "../../../System/Process.h"

class FNamePoolParserClass
{
public:
    FNamePoolParserClass() {};
    ~FNamePoolParserClass() {};

    template <class U>
    bool GetFNameStringByID(U InputValue, std::string& RetStr, bool IsIndex);

    void Thread_FindString(const std::string InputString, size_t Start, size_t End);

    bool FindStringFromFNamePool(const std::string& InputString);

    void ParseFNamePool();
private:
};
inline FNamePoolParserClass FNameParser = FNamePoolParserClass();




template <class U>
bool FNamePoolParserClass::GetFNameStringByID(U InputValue, std::string& RetStr, bool IsIndex) {
    int ID;
    long long int TempValue = InputValue;
    long long int FNamePoolID_LargerThanExpected = 0;

    //輸入處理
    if (IsIndex) {
        FNamePoolID_LargerThanExpected = (TempValue >> 0x20); //除非 FNamePool 的數量大於 0xFFFF      //位移大小要小於變數的大小，否則直接位移太大會被判定無效，Ex: int 最多位移 >> 31
        ID = InputValue & 0xFFFFFFFF;
    }
    else {
        //IS Address
        FNamePoolID_LargerThanExpected = MemMgr.MemReader.ReadMem<int>(InputValue + 4);
        ID = MemMgr.MemReader.ReadMem<int>(InputValue);
    }

    //如果已處理過，則直接返回結果
    if (!(StorageMgr.FNamePoolDict.Get(ID).empty())) {
        if (FNamePoolID_LargerThanExpected > 0) RetStr = StorageMgr.FNamePoolDict.Get(ID) + "_" + std::to_string(FNamePoolID_LargerThanExpected - 1);
        else RetStr = StorageMgr.FNamePoolDict.Get(ID);
        return true;
    }

    //基本設定
    int FNamePoolIndex = ID >> 0x10;
    int FNameOffest = (ID & 0xFFFF) << 1;            //後面這個 2 根據不同遊戲可能會有所不同，但都是 2 的倍數
    DWORD_PTR FNamePoolAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(StorageMgr.FNamePoolBaseAddress.Get() + StorageMgr.FNamePoolFirstPoolOffest.Get() + FNamePoolIndex * ProcessInfo::ProcOffestAdd);
    int16_t StringLen = MemMgr.MemReader.ReadMem<int16_t>(FNamePoolAddress + FNameOffest);
    StringLen = StringLen >> 6;                             //字串中的前兩 bytes 整數，往右 shift 6 位後，就是後面的字串長度
    if (StringLen > 200 or StringLen < 1) return false;     //字串太長則捨去  // StringLen 可能會為 1，Ex: X、Y、Z 座標的 Name

    //判斷字串是否是寬字元        // 這邊應該是要判斷一個的 byte 的第一個 bit 是否為 1
    //BYTE FirstByte = MemMgr.ReadMem<BYTE>(FNamePoolAddress + FNameOffest).first;
    //bool IsWChar;
    //if (FirstByte == 1) {
    //    IsWChar = true;
    //    StringLen *= 2;                     //不確定尾八需不需要減一
    //}

    //尋找 String 開頭第一個元素的偏移量，並取的目標 String
    std::string Name;
    char* Buffer = new char[StringLen + 10];
    if (!StorageMgr.FNamePoolFirstStringOffest.IsInitialized()) {
        if (ID > 0 and ID < 7 and StringLen > 10 and StringLen < 15) {  //表示剛進 FNamePool，且忽略掉第一個 None 字串，找到第二個名為 "ByteProperty" 的字串位置
            for (int i = 0x2; i < 0x20; i++) {
                std::memset(Buffer, 0, sizeof(Buffer));

                MemMgr.MemReader.ReadBytes(FNamePoolAddress + FNameOffest + i, (BYTE*)Buffer, StringLen);
                Buffer[StringLen] = '\0';
                Name = Buffer;

                if (Name.find("ByteProperty") != std::string::npos) {
                    StorageMgr.FNamePoolFirstStringOffest.Set(i);
                    break;
                }
            }
        }
        if (!StorageMgr.FNamePoolFirstStringOffest.IsInitialized()) {
            delete[] Buffer;
            return false;
        }
    }


    std::memset(Buffer, 0, sizeof(Buffer));
    MemMgr.MemReader.ReadBytes(FNamePoolAddress + FNameOffest + StorageMgr.FNamePoolFirstStringOffest.Get(), (BYTE*)Buffer, StringLen);
    Buffer[StringLen] = '\0';
    Name = Buffer;
    delete[] Buffer;
    if (Name.length() < 1) return false;       //最後實際讀取出的字串長度如果小於 1，表示一樣不是在正確的位置讀取的 //前面確認讀取的字串長度，這邊確認實際讀取的字串長度

    //Return
    if (FNamePoolID_LargerThanExpected > 0) {
        RetStr = Name + "_" + std::to_string(FNamePoolID_LargerThanExpected - 1);

    }
    else {
        StorageMgr.FNamePoolDict.Set(ID, Name);
        RetStr = Name;
    }

    return true;
}