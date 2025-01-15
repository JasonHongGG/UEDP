#pragma once
#include <Windows.h>
#include <string>
#include "DumperUtils.h"
#include "Parser.h"
#include "UEOffset.h"
#include "../Interface/ObjectInterface.h"
#include "../Storage/StorageManager.h"
#include "../../System/Memory.h"

class UEOffsetAutoConfig
{
public:
    int64_t TempFNameID;
    std::string TempFNameStr = "";
    std::pair<DWORD_PTR, bool> TempReadMemResult;
    ObjectData TempObj;

    void FindBasicInfoOffset();

    void FindSuperAndMemberOffset(DWORD_PTR GameEngineObjectAddress, DWORD_PTR& EngineORActorObjectAddress);

    void FindProperty(DWORD_PTR EngineORActorObjectAddress);

    void FindOffsetOffset(DWORD_PTR EngineORActorObjectAddress);

    void FindBitMaskOffset();

    void AutoConfig();
private:
};

inline UEOffsetAutoConfig AutoConfig = UEOffsetAutoConfig();