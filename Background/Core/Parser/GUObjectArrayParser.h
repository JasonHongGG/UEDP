#pragma once
#include <Windows.h>
#include <string>
#include "../Object.h"
#include "../../../State/GUIState.h"
#include "../../Storage/StorageManager.h"
#include "../../../Utils/Include/BS_thread_pool.hpp"
#include "../../../Utils/Utils.h"
#include "../../../Utils/Env.h"

#include "../../../System/Memory.h"
#include "../../../System/Process.h"

class GUObjectArrayParserClass
{
public:
    GUObjectArrayParserClass() {};
    ~GUObjectArrayParserClass() {};

    enum class GUObjectExecuteMode {
        Parse = 0,
        Search,
    };
    size_t MaxObjectArray = Const::MaxObjectArray;
    size_t MaxObjectQuantity = Const::MaxObjectQuantity;
    void Thread_SearchAllObject(DWORD_PTR Address, size_t Start, size_t End, size_t GUObjectArray_ArrEleSize, std::string FullName);

    DWORD_PTR ParseGUObjectArray(GUObjectExecuteMode Mode = GUObjectExecuteMode::Parse, std::string FullName = "");

    DWORD_PTR FindObjectByFullName(std::string FullName);
private:
};
inline GUObjectArrayParserClass GUObjectParser = GUObjectArrayParserClass();