#include "ProcessModule.h"

HMODULE ProcessModule::hMainMoudle = 0;

// Module (�n�`�N���O���ǵ{���|���h�ӬۦP�W�٪� Exe�BDLL�A��p RPG Maker ���C��)�A�i��|�ݭn�ίS�O���������A���O�O����e�γ̦h��
HMODULE ProcessModule::GetModule(size_t PID, const wchar_t* name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule �M szExePath �O Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (!_wcsicmp(moduleEntry.szModule, name)) {
                CloseHandle(hSnapshot);
                ProcessModule::hMainMoudle = moduleEntry.hModule;
                return moduleEntry.hModule;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    return 0;
}

DWORD_PTR ProcessModule::GetModuleBaseAddress(size_t PID, LPCTSTR name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32 moduleEntry32;            // MODULEENTRY32 �N�O MODULEENTRY32W �ڥ��b�d�A���O Unicode
    moduleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &moduleEntry32)) {
        do {
            //_tprintf(TEXT("\n\n MODULE NAME: %s"), moduleEntry32.szModule);     // �Ψ� print Unicode ��
            if (_tcsicmp(moduleEntry32.szModule, name) == 0) {
                CloseHandle(hSnapshot);
                return (DWORD_PTR)moduleEntry32.modBaseAddr;
            }
        } while (Module32Next(hSnapshot, &moduleEntry32));
    };

    CloseHandle(hSnapshot);
    return 0;
}

DWORD ProcessModule::GetModuleSize(HANDLE hProcess, HMODULE hModule)
{
    MODULEINFO moduleInfo;
    if (GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo)))
        return moduleInfo.SizeOfImage;
    return 0;
}

DWORD ProcessModule::GetModuleSizeByPidAndName(size_t PID, LPCTSTR name) {
    DWORD dwSize = 0;

    do {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
        if (hSnapshot == INVALID_HANDLE_VALUE) { continue; }

        MODULEENTRY32 ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &ModuleEntry32)) {
            do {
                if (_tcsicmp(ModuleEntry32.szModule, name) == 0) {
                    dwSize = ModuleEntry32.modBaseSize;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }

        CloseHandle(hSnapshot);
    } while (!dwSize);

    return dwSize;
}

void ProcessModule::PrintAllModuleInfo(size_t PID)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule �M szExePath �O Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {

            printf("[ Module Name ] %ls\n", moduleEntry.szModule);
            printf("[ Module Base Address ] %p \t [Moduel Size] %5ld\n", (void*)(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);


        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
}

bool ProcessModule::InMoudle(size_t PID, DWORD_PTR Address)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule �M szExePath �O Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (Address >= reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr) && Address <= (reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr) + moduleEntry.modBaseSize)) {
                //printf("[ Find !!!! ] \n");
                //printf("[ Module Name ] %ls\n", moduleEntry.szModule);
                //printf("[ Module Base Address ] %p \t [Moduel Size] %5d\n", reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);

                CloseHandle(hSnapshot);
                return true;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    return false;
}