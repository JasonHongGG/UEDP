#pragma once

namespace EventHandler
{
    inline void UpdataPackageObjectList(std::string PackageName)
    {
        const ProcessState State = PackageViwerState.ObjectDataUpdateEvent.State;
        if (IsEventAcceptable(State)) {
            PackageViwerState.ObjectDataUpdateEvent.PackageName = PackageName;
            PackageViwerState.ObjectDataUpdateEvent.State = ProcessState::Start;
        }
    }

    inline void OpenPackageObjectTab(std::string ObjectName, DWORD_PTR Address)
    {
        const ProcessState State = PackageViwerState.ObjectContentCreatEvent.State;
        if (IsEventAcceptable(State)) {
            PackageViwerState.ObjectContentCreatEvent.Name = ObjectName;
            PackageViwerState.ObjectContentCreatEvent.Address = Address;
            PackageViwerState.ObjectContentCreatEvent.State = ProcessState::Start;
        }
    }

    inline void UpdataGlobalSearchObject(std::string SearchString) {
        const ProcessState State = PackageViwerState.SearchUpdateEvent.State;
        if (IsEventAcceptable(State)) {
            PackageViwerState.SearchUpdateEvent.SearchStr = SearchString;
            PackageViwerState.SearchUpdateEvent.State = ProcessState::Start;
        }
    }

    inline void ShowGlobalSearchPackageObjectResult(std::string PackageName, std::string TabName, std::string ObjectName, DWORD_PTR Address)
    {
        const ProcessState State = PackageViwerState.SearchShowEvent.State;
        if (IsEventAcceptable(State)) {
            PackageViwerState.SearchShowEvent.PackageName = PackageName;
            PackageViwerState.SearchShowEvent.TabName = TabName;
            PackageViwerState.SearchShowEvent.ObjectName = ObjectName;
            PackageViwerState.SearchShowEvent.Address = Address;
            PackageViwerState.SearchShowEvent.State = ProcessState::Start;
        }
    }
}