#pragma once
#include "Interface.h"

class StorageManager
{
public:
	Property<size_t> UEVersion;

	Property<size_t> FNamePoolBaseAddress;
	Property<size_t> FNamePoolFirstPoolOffest;

	Property<size_t> GUObjectArrayBaseAddress;
	Property<size_t> GUObjectArrayElementSize;

	Property<size_t> GWorldBaseAddress;
	
private:

};
inline StorageManager StorageMgr = StorageManager();