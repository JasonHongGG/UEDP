#pragma once
#include <Windows.h>
#include <vector>
#include <string>

class MonoAssemblyImage
{
public:
	DWORD_PTR Handle = 0x0;
public:
	MonoAssemblyImage(DWORD_PTR handle) : Handle(handle) {};
};

class MonoImage
{
public:
	DWORD_PTR AssemblyImageHandle = 0x0;
	DWORD_PTR Handle = 0x0;
	DWORD_PTR ImagesAddress = 0x0;
	std::string Name = "";
	MonoImage(MonoAssemblyImage AssemblyImage, std::string name, DWORD_PTR address) : AssemblyImageHandle(AssemblyImage.Handle), Name(name), Handle(address) {};
};
