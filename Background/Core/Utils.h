#pragma once
#include <Windows.h>
#include "../Storage/StorageManager.h"
#include "../../System/FileAttribute.h"

class DumperUtilsSet
{
public:
    DumperUtilsSet() {}
    ~DumperUtilsSet() {}

    template <class U>
    DWORD_PTR CheckValue(DWORD_PTR Address, size_t Size, U InputValue, size_t Type, bool literal = false);

    bool GetUEVersion();

private:

};

inline DumperUtilsSet DumperUtils = DumperUtilsSet();