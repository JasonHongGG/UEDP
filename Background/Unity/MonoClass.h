#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "UnityImage.h"

class MonoClass
{
public:
	DWORD_PTR Handle = 0x0;
	MonoImage* Image = nullptr;
	std::string ClassName = "";
	std::string ClassNamespace = "";

	MonoClass(MonoImage* image, DWORD_PTR handle, std::string className, std::string classNamespace) : Image(image), Handle(handle), ClassName(className), ClassNamespace(classNamespace) {};
};