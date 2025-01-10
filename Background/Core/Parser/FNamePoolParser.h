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

    //��J�B�z
    if (IsIndex) {
        FNamePoolID_LargerThanExpected = (TempValue >> 0x20); //���D FNamePool ���ƶq�j�� 0xFFFF      //�첾�j�p�n�p���ܼƪ��j�p�A�_�h�����첾�Ӥj�|�Q�P�w�L�ġAEx: int �̦h�첾 >> 31
        ID = InputValue & 0xFFFFFFFF;
    }
    else {
        //IS Address
        FNamePoolID_LargerThanExpected = MemMgr.MemReader.ReadMem<int>(InputValue + 4);
        ID = MemMgr.MemReader.ReadMem<int>(InputValue);
    }

    //�p�G�w�B�z�L�A�h������^���G
    if (!(StorageMgr.FNamePoolDict.Get(ID).empty())) {
        if (FNamePoolID_LargerThanExpected > 0) RetStr = StorageMgr.FNamePoolDict.Get(ID) + "_" + std::to_string(FNamePoolID_LargerThanExpected - 1);
        else RetStr = StorageMgr.FNamePoolDict.Get(ID);
        return true;
    }

    //�򥻳]�w
    int FNamePoolIndex = ID >> 0x10;
    int FNameOffest = (ID & 0xFFFF) << 1;            //�᭱�o�� 2 �ھڤ��P�C���i��|���Ҥ��P�A�����O 2 ������
    DWORD_PTR FNamePoolAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(StorageMgr.FNamePoolBaseAddress.Get() + StorageMgr.FNamePoolFirstPoolOffest.Get() + FNamePoolIndex * ProcessInfo::ProcOffestAdd);
    int16_t StringLen = MemMgr.MemReader.ReadMem<int16_t>(FNamePoolAddress + FNameOffest);
    StringLen = StringLen >> 6;                             //�r�ꤤ���e�� bytes ��ơA���k shift 6 ���A�N�O�᭱���r�����
    if (StringLen > 200 or StringLen < 1) return false;     //�r��Ӫ��h�˥h  // StringLen �i��|�� 1�AEx: X�BY�BZ �y�Ъ� Name

    //�P�_�r��O�_�O�e�r��        // �o�����ӬO�n�P�_�@�Ӫ� byte ���Ĥ@�� bit �O�_�� 1
    //BYTE FirstByte = MemMgr.ReadMem<BYTE>(FNamePoolAddress + FNameOffest).first;
    //bool IsWChar;
    //if (FirstByte == 1) {
    //    IsWChar = true;
    //    StringLen *= 2;                     //���T�w���K�ݤ��ݭn��@
    //}

    //�M�� String �}�Y�Ĥ@�Ӥ����������q�A�è����ؼ� String
    std::string Name;
    char* Buffer = new char[StringLen + 10];
    if (!StorageMgr.FNamePoolFirstStringOffest.IsInitialized()) {
        if (ID > 0 and ID < 7 and StringLen > 10 and StringLen < 15) {  //��ܭ�i FNamePool�A�B�������Ĥ@�� None �r��A���ĤG�ӦW�� "ByteProperty" ���r���m
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
    if (Name.length() < 1) return false;       //�̫���Ū���X���r����צp�G�p�� 1�A��ܤ@�ˤ��O�b���T����mŪ���� //�e���T�{Ū�����r����סA�o��T�{���Ū�����r�����

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