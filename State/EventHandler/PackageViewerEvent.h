#pragma once

namespace EventHandler
{
    inline void UpdataPackageObjectList(std::string PackageName)
    {
        const ProcessState State = PackageViwerState.ObjectDataUpdateEvent;
        if (IsEventAcceptable(State)) {
            PackageViwerState.ObjectDataUpdateMsg.PackageName = PackageName;
            PackageViwerState.ObjectDataUpdateEvent = ProcessState::Start;
        }
    }

    inline void OpenPackageObjectTab(std::string ObjectName, DWORD_PTR Address)
    {
        const ProcessState State = PackageViwerState.ObjectContentCreatEvent;
        if (IsEventAcceptable(State)) {
            PackageViwerState.ObjectContentCreatMsg.Name = ObjectName;
            PackageViwerState.ObjectContentCreatMsg.Address = Address;
            PackageViwerState.ObjectContentCreatEvent = ProcessState::Start;
        }
    }

    inline void UpdataGlobalSearchObject(std::string SearchString) {
        const ProcessState State = PackageViwerState.SearchUpdateEvent;
        if (IsEventAcceptable(State)) {
            PackageViwerState.SearchUpdateMsg.SearchStr = SearchString;
            PackageViwerState.SearchUpdateEvent = ProcessState::Start;
        }
    }

    inline void ShowGlobalSearchPackageObjectResult(std::string PackageName, std::string TabName, std::string ObjectName, DWORD_PTR Address)
    {
        const ProcessState State = PackageViwerState.SearchShowEvent;
        if (IsEventAcceptable(State)) {
            PackageViwerState.SearchShowMsg.PackageName = PackageName;
            PackageViwerState.SearchShowMsg.TabName = TabName;
            PackageViwerState.SearchShowMsg.ObjectName = ObjectName;
            PackageViwerState.SearchShowMsg.Address = Address;
            PackageViwerState.SearchShowEvent = ProcessState::Start;
        }
    }
}