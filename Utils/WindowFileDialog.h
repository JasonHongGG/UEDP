#pragma once
#include <windows.h>
#include <string>
#include "./utils.h"

class WindowFileDialog 
{
public:

    std::string Open()
    {
        OPENFILENAME ofn;     
        wchar_t szFile[260];    
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;   // 父窗口可以設定為 NULL
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // 顯示文件選擇對話框
        if (GetOpenFileName(&ofn))
            return std::string(Utils.UnicodeToANSI(ofn.lpstrFile));
        return "";
    }

    std::string Save()
    {
        OPENFILENAME ofn;       
        wchar_t szFile[260];     
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;   // 父窗口可以設定為 NULL
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetSaveFileName(&ofn)) {
            std::string result = std::string(Utils.UnicodeToANSI(ofn.lpstrFile));
            if(result.back() == '\0') result.pop_back();
            result += ".json";
            return result;
        }
        return "";
    }
};

inline WindowFileDialog winFileDialog = WindowFileDialog();