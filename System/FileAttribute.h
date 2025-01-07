#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <winver.h>
#include "Process.h"


class FileAttributeManager
{
public:
    bool QueryValue(const std::string& ValueName, std::string& RetStr, bool RetNum = false);

    bool	GetFileDescription(std::string& RetStr);   //�����󻡩�
    bool	GetFileVersion(std::string& RetStr);	   //�����󪩥�	// RetNum = std::stoi(RetStr);
    bool	GetInternalName(std::string& RetStr);	   //��������W��
    bool	GetCompanyName(std::string& RetStr);	   //������q�W��
    bool	GetLegalCopyright(std::string& RetStr);    //������v
    bool	GetOriginalFilename(std::string& RetStr);  //�����l���W
    bool	GetProductName(std::string& RetStr);	   //������~�W��
    bool	GetProductVersion(std::string& RetStr);    //������~����
private:
};

inline FileAttributeManager FileAttrMgr = FileAttributeManager();



//�ϥΤ覡
/*
int RetNum123;
std::string RetStr123;

FileAttrMgr.GetFileDescription(RetStr123);
std::cout << "[FileDescription]    " << RetStr123 << std::endl;

FileAttrMgr.GetFileVersion(RetStr123);
printf("[FileVersion] %s\n", RetStr123.c_str());
std::cout << "[FileVersion]    " << RetStr123 << std::endl;

FileAttrMgr.GetFileVersionNumber(RetNum123);
printf("[FileVersion Number] %d\n", RetNum123);

FileAttrMgr.GetInternalName(RetStr123);
std::cout << "[InternalName]    " << RetStr123 << std::endl;

FileAttrMgr.GetCompanyName(RetStr123);
std::cout << "[CompanyName]    " << RetStr123 << std::endl;

FileAttrMgr.GetLegalCopyright(RetStr123);
std::cout << "[LegalCopyright]    " << RetStr123 << std::endl;

FileAttrMgr.GetOriginalFilename(RetStr123);
std::cout << "[OriginalFilename]    " << RetStr123 << std::endl;

FileAttrMgr.GetProductName(RetStr123);
std::cout << "[ProductName]    " << RetStr123 << std::endl;

FileAttrMgr.GetProductVersion(RetStr123);
std::cout << "[ProductVersion]    " << RetStr123 << std::endl;

*/