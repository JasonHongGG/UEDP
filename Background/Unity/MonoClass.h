#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <ranges>
#include "MonoImage.h"
#include "MonoUtils.h"

class MonoClassAPI;
class MonoClass;
class MonoField;


class MonoField
{
public:
	DWORD_PTR Handle = 0x0;
	MonoClass* Class = nullptr;
	std::string Name = "";
	int Flags = 0;
	DWORD_PTR TypeAddress = 0;
	std::string TypeName = "";
	DWORD_PTR Address = 0x0;

	MonoField(MonoClass* _class, DWORD_PTR handle, std::string name) : Class(_class), Handle(handle), Name(name) {}

	int GetFlags();

	DWORD_PTR GetTypeAddress();

	std::string GetTypeName();

	DWORD_PTR GetAddress();

	DWORD_PTR GetValue();
};




class MonoClass
{
public:
	MonoClassAPI* ClassAPI;
	DWORD_PTR Handle = 0x0;
	MonoImage* Image = nullptr;
	DWORD_PTR Instance = 0x0;
	DWORD_PTR VTable = 0x0;
	std::string ClassName = "";
	std::string ClassNamespace = "";

	MonoClass(MonoClassAPI* api, MonoImage* image, DWORD_PTR handle, std::string className, std::string classNamespace)
		: ClassAPI(api), Image(image), Handle(handle), ClassName(className), ClassNamespace(classNamespace) {
	}

	MonoField* FindField(std::string FieldName);

	DWORD_PTR GetVtable();
};




class MonoClassAPI
{
public:
	MonoImageAPI* ImageAPI;
	MonoNativeFuncSet* FunctSet;
	std::vector<DWORD_PTR>* ThreadFunctionList;

	MonoClassAPI(MonoImageAPI* imageAPI, MonoNativeFuncSet* functSet, std::vector<DWORD_PTR>* threadFunctionList)
		: ImageAPI(imageAPI), FunctSet(functSet), ThreadFunctionList(threadFunctionList) {
	}

	MonoClass* FindClassInImageByName(std::string ImageName, std::string ClassName);

	MonoField* FindFieldInClassByName(MonoClass* Class, std::string FieldName);

	DWORD_PTR GetStaticFieldAddress(MonoClass* Class, MonoField* Field);

	template <typename T>
	T GetStaticFieldValue(MonoClass* Class, MonoField* Field, std::string TypeName);

	bool IsStatic(MonoField* Field);
};

