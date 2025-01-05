#pragma once
#define WIN32_LEAN_AND_MEAN 
#include "Process/ProcessInfo.h"
#include "Process/ProcessModule.h"
#include "Process/ProcessWindow.h"

class Process
{
public:
    // Parameter
    ProcessInfo infoMgr;
    ProcessModule moduleMgr;
    ProcessWindow windowMgr;

    Process() {
        infoMgr = ProcessInfo();
        moduleMgr = ProcessModule();
        windowMgr = ProcessWindow();
    }
    ~Process() {}
private:
};

inline Process ProcMgr = Process();