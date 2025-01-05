#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <chrono>
#include "Utils.h"


std::string UtilsSet::ImplicitStringWrapper(const char* format, va_list args) {
    static char Msg[128] = "";
    vsnprintf(Msg, sizeof(Msg), format, args);
    return std::string(Msg);
}

std::string UtilsSet::StringWrapper(const char* format, ...)
{
    if (format) {
        va_list args;
        va_start(args, format);
        std::string result = ImplicitStringWrapper(format, args);
        va_end(args);
        return result;
    }
    return "";
}

// Normal Tool
void UtilsSet::CopyToClipBoard(const std::string& Str)
{
    const std::wstring UnicodeStr = ANSIToUnicode(Str.c_str());

    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();

    const auto size = (UnicodeStr.size() + 1) * sizeof(wchar_t);
    const auto hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hGlobal)
    {
        CloseClipboard();
        return;
    }

    const auto lpGlobal = GlobalLock(hGlobal);
    if (!lpGlobal)
    {
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    memcpy(lpGlobal, UnicodeStr.c_str(), size);
    GlobalUnlock(hGlobal);

    SetClipboardData(CF_UNICODETEXT, hGlobal); //CF_UNICODETEXT  CF_TEXT
    CloseClipboard();
}


// Bit Operation
const std::map<int, uint8_t> BitMaskTable = {
    { 0 , 0x01 },
    { 1 , 0x02 },
    { 2 , 0x04 },
    { 3 , 0x08 },
    { 4 , 0x10 },
    { 5 , 0x20 },
    { 6 , 0x40 },
    { 7 , 0x80 }
};

int UtilsSet::FindRightMostSetBit(int num) {
    int position = 0;
    while (num) {
        if (num & 1) return position;

        num >>= 1;  // shift 1 bit
        ++position;
    }
    return -1;
}


// String
bool UtilsSet::ContainStr(std::string& Str, std::string TargetStr)
{
    if (Str.find(TargetStr) != std::string::npos) return true;
    else return false;
}

bool UtilsSet::IsHex(const std::string& str) {
    if (str.empty()) return false;

    for (char c : str) {
        if (!std::isxdigit(c)) { // 檢查是否為十六進制數字
            return false;
        }
    }
    return true;
}

std::vector<std::string> UtilsSet::GetTokens(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string FindSubStr(std::string input) {
    // 定義要匹配的字串 EX : std::string input = "mov rax, qword [rsp+0x30]";

    std::regex pattern("\\[([^\\]]+)\\]");

    // 進行匹配
    std::smatch matches;
    if (std::regex_search(input, matches, pattern)) {
        std::string match = matches[1].str();
        std::cout << "匹配到的部分: " << match << std::endl;
        return match;
    }
    else {
        std::cout << "沒有找到匹配的部分" << std::endl;
        return "";
    }
}

std::string UtilsSet::HexToString(size_t Hex) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << Hex; // 轉換成16進位數字的字串
    std::string HexStr = ss.str();
    return HexStr;
}

size_t UtilsSet::StringToHex(std::string HexStr) {
    if (HexStr.find("0x") != std::string::npos) HexStr.erase(HexStr.find("0x"), 2);
    if (HexStr.find("0X") != std::string::npos) HexStr.erase(HexStr.find("0X"), 2);

    return std::stoi(HexStr, nullptr, 16);
}

void UtilsSet::StringToLower(std::string& Str) {
    for (char& c : Str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

std::string UtilsSet::rStringToLower(std::string Str) {
    for (char& c : Str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return Str;
}


// Transform String Type
// ansi 為單 bytes，unicode 為多 bytes 可表示所有字符， UTF8 為 unicode 的一種編碼方式
std::string UtilsSet::UnicodeToUTF8(const wchar_t* UnicodeString) {
    // 計算 UTF8 所需的字串長度，並且轉換
    int utf8_length = WideCharToMultiByte(CP_UTF8, 0, UnicodeString, -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_length, 0);
    WideCharToMultiByte(CP_UTF8, 0, UnicodeString, -1, &utf8_str[0], utf8_length, nullptr, nullptr);

    return utf8_str;
}

std::wstring UtilsSet::UTF8ToUnicode(const char* utf8String) {
    int unicodeLength = MultiByteToWideChar(CP_ACP, 0, utf8String, -1, nullptr, 0);
    std::wstring unicodeString(unicodeLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, utf8String, -1, &unicodeString[0], unicodeLength);
    return unicodeString;
}

std::string UtilsSet::UnicodeToANSI(const wchar_t* UnicodeString) {
    // 計算 ANSI 所需的字串長度，並且轉換
    int ansi_length = WideCharToMultiByte(CP_ACP, 0, UnicodeString, -1, NULL, 0, NULL, NULL);
    std::string ansi_str(ansi_length, 0);
    WideCharToMultiByte(CP_ACP, 0, UnicodeString, -1, &ansi_str[0], ansi_length, NULL, NULL);

    return ansi_str;
}

std::wstring UtilsSet::ANSIToUnicode(const char* ansiString) {
    int unicodeLength = MultiByteToWideChar(CP_ACP, 0, ansiString, -1, nullptr, 0);
    std::wstring unicodeString(unicodeLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString, -1, &unicodeString[0], unicodeLength);
    return unicodeString;
}

std::string UtilsSet::ANSIToUTF8(const char* ansiString) {
    return UnicodeToUTF8(ANSIToUnicode(ansiString).c_str());
}

std::string UtilsSet::UTF8ToANSI(const char* utf8String) {
    return UnicodeToUTF8(ANSIToUnicode(utf8String).c_str());
}


// Time
std::string UtilsSet::GetCurTime()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}

std::string UtilsSet::GetTime(bool GetCurrentTime, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime, std::chrono::steady_clock::time_point& CurTime)
{
    std::chrono::duration<double> Duration;
    if (GetCurrentTime) {	// Current Time
        CurTime = std::chrono::high_resolution_clock::now();
        Duration = CurTime - StartTime;
    }
    else {					// Last Record Time
        Duration = EndTime - StartTime;
    }

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(Duration);
    Duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(Duration);
    Duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(Duration);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes.count() << ":"
        << std::setw(2) << std::setfill('0') << seconds.count() << ":"
        << std::setw(3) << std::setfill('0') << milliseconds.count();

    return oss.str();
}

void UtilsSet::TimerSwitch(bool Activate, bool* TimerActivate, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime)
{
    if (Activate) {
        StartTime = std::chrono::high_resolution_clock::now();
        *(TimerActivate) = true;
    }
    else {
        EndTime = std::chrono::high_resolution_clock::now();
        *(TimerActivate) = false;
    }
}