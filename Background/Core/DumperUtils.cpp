#include "DumperUtils.h"

template <class U>
DWORD_PTR DumperUtilsSet::CheckValue(DWORD_PTR Address, size_t Size, U Value, size_t Type, bool StrFullCompare)
{
    // �γ~ : �b���w Address ���� Size �d�򤺡A��M InputValue
    // �p�G�����h�^�ǧ�쪺 Offest�A�ϫh�� NULL

    //�ˬd Address �O�_����
    if (!MemMgr.MemReader.IsPointer(Address)) return NULL;

    // Read Bytes Form Address
    BYTE* BytesBuffer = new BYTE[Size + 0x10];
    MemMgr.MemReader.ReadBytes(Address, BytesBuffer, Size);

    // �ܼ�
    std::vector<unsigned char> Data;
    int TempValue, InputValue_1 = 0, InputValue_2 = 0;
    std::string InputString;

    if (std::is_same<U, std::string>::value) {      //�Ȯɥ���@�i�঳ "~" ���r�B�ݳB�z
        InputString = Value;
        //InputValue_1 = std::stoi(InputString.substr(InputString.find("~")+1));
        //InputValue_2 = std::stoi(InputString.substr(0, InputString.find("~")));;
    }
    else if (std::is_same<U, int>::value) {
        if constexpr (std::is_convertible_v<U, int>) {
            InputValue_1 = static_cast<int>(Value);
        }
    }

    // =================
    if (Type == 1) {  // �r��
        std::string FName;

        for (size_t i = 0; i < Size; i += 4) {
            Data.clear();

            // 4 bytes ��Ʀr
            for (size_t m = 0; m < 4; ++m)
                Data.push_back(*(BytesBuffer + i + m));
            TempValue = Utils.BytesToNum(Data);

            // ���ը��r��
            if (FNameParser.GetFNameStringByID(TempValue, FName, true)) {
                // FName ���r��A�B FName �M InputString �����ǰt(StrFullCompare == false)�B����ǰt(StrFullCompare == true)
                if (!FName.empty()) {
                    if ((!StrFullCompare and FName.find(InputString) != std::string::npos) or (FName == InputString)) {    //�����ǰt�γ����ǰt
                        delete[] BytesBuffer;
                        return Address + i;
                    }
                }
            }
        }
    }
    else if (Type == 2 || Type == 4 || Type == 8) {      //�Ʀr // ��ӡB�|�ӡB�K�� bytes �@��
        for (size_t i = 0; i < Size; i += Type) {
            Data.clear();
            for (size_t m = 0; m < Type; ++m) {
                Data.push_back(*(BytesBuffer + i + m));
            }
            TempValue = Utils.BytesToNum(Data);
            // �p�G TempValue �M InputValue_1 �۵�
            // �άO InputValue_2 �s�b�A�BInputValue_2 >= TempValue >= InputValue_1
            if (TempValue == InputValue_1 || (InputValue_2 && TempValue >= InputValue_1 && InputValue_2 >= InputValue_1)) {
                delete[] BytesBuffer;
                return Address + i;
            }
        }
    }
    delete[] BytesBuffer;
    return NULL;
}

bool DumperUtilsSet::GetUEVersion() {
    if (StorageMgr.UEVersion.IsInitialized()) {
        printf("[ UE Version Exist ] %d\n\n", (int)StorageMgr.UEVersion.Get());
        return true;
    }

    std::string UEVersionStr = "";
    FileAttrMgr.GetFileVersion(UEVersionStr);
    StorageMgr.UEVersion.Set(std::stoi(UEVersionStr));
    printf("[ UE Version ] %s\n\n", UEVersionStr.c_str());
    return true;
}



// �㦡��Ҥ� (�� template �i�H�w�q�b cpp)
template DWORD_PTR DumperUtilsSet::CheckValue<int>(DWORD_PTR, size_t, int, size_t, bool);
template DWORD_PTR DumperUtilsSet::CheckValue<std::string>(DWORD_PTR, size_t, std::string, size_t, bool);