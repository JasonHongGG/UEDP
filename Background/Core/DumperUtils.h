#pragma once
#include <Windows.h>
#include "Parser.h"
#include "../Storage/StorageManager.h"
#include "../../System/Memory.h"
#include "../../System/FileAttribute.h"
#include "../../Utils/Utils.h"

class DumperUtilsSet
{
public:
    DumperUtilsSet() {}
    ~DumperUtilsSet() {}

    template <class U>
    DWORD_PTR CheckValue(DWORD_PTR Address, size_t Size, U Value, size_t Type, bool StrFullCompare = false);

    bool GetUEVersion();

private:

};

inline DumperUtilsSet DumperUtils = DumperUtilsSet();
