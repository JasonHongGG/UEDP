#pragma once
#include <string>

#include "../GUI/Config/NotificationConfig.h"

#include "EventHandler/Utils.h"
#include "EventHandler/LoadAndSaveSettingEvent.h"
#include "EventHandler/ConsoleEvent.h"

#include "GlobalState.h"
#include "GUIState.h"


namespace EventHandler
{
    inline void NotifyEvent(NotificationConfig::NotiyType Type, std::string Title, std::string Content = "...", float DismissTime = NotificationConf.DismissTimeBar)
    {
        const ProcessState State = NotificationState.NotifyEvent;

        if (IsEventAcceptable(State)) {
            NotificationConf.Type = Type;
            NotificationConf.Title = Title;
            NotificationConf.Content = Content;
            NotificationConf.DismissTime = DismissTime;
            NotificationState.NotifyEvent = ProcessState::Start;
            if (!Title.empty() or !Content.empty()) printf((Title + (!Title.empty() ? " " : "") + (Content = "..." ? "" : Content) + "\n").c_str());
        }
    }

 

    //// ============================================================================================================
    //// Package Inspector
    //void UpdataPackageObjectList(std::string PackageName)
    //{
    //    const ProcessState State = PackageViwerConf.PackageObjectData.UpdateEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        PackageViwerConf.PackageObjectData.UpdateEvent.PackageName = PackageName;
    //        PackageViwerConf.PackageObjectData.UpdateEvent.State = ProcessState::Start;
    //    }
    //}

    //void OpenPackageObjectTab(std::string ObjectName, DWORD_PTR Address)
    //{
    //    const ProcessState State = PackageViwerConf.PackageObjectContent.CreatEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        PackageViwerConf.PackageObjectContent.CreatEvent.Name = ObjectName;
    //        PackageViwerConf.PackageObjectContent.CreatEvent.Address = Address;
    //        PackageViwerConf.PackageObjectContent.CreatEvent.State = ProcessState::Start;
    //    }
    //}

    //void UpdataGlobalSearchObject(std::string SearchString) {
    //    const ProcessState State = PackageViwerConf.GlobalPackageObject.UpdateEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        PackageViwerConf.GlobalPackageObject.UpdateEvent.SearchStr = SearchString;
    //        PackageViwerConf.GlobalPackageObject.UpdateEvent.State = ProcessState::Start;
    //    }
    //}

    //void ShowGlobalSearchPackageObjectResult(std::string PackageName, std::string TabName, std::string ObjectName, DWORD_PTR Address)
    //{
    //    const ProcessState State = PackageViwerConf.GlobalPackageObject.ShowEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        PackageViwerConf.GlobalPackageObject.ShowEvent.PackageName = PackageName;
    //        PackageViwerConf.GlobalPackageObject.ShowEvent.TabName = TabName;
    //        PackageViwerConf.GlobalPackageObject.ShowEvent.ObjectName = ObjectName;
    //        PackageViwerConf.GlobalPackageObject.ShowEvent.Address = Address;
    //        PackageViwerConf.GlobalPackageObject.ShowEvent.State = ProcessState::Start;
    //    }
    //}

    //// ============================================================================================================
    //// Inspector
    //void EditorGetFName(DWORD_PTR Address, int FNameID)
    //{
    //    const ProcessState State = EditorMemMgr.GetFNameEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        EditorMemMgr.GetFNameEvent.Address = Address;
    //        EditorMemMgr.GetFNameEvent.FNameID = FNameID;
    //        EditorMemMgr.GetFNameEvent.State = ProcessState::Start;
    //    }
    //}
    //
    //bool OpenObjectContentPage(DWORD_PTR Address, std::string Type, std::string Name, bool EditorEnable, bool IsInstance)
    //{
    //    if (UpdateObjectContentPage(Address, Type, Name, EditorEnable, IsInstance, Address, 0)) {         // 創建新 Page 時不可能是 array、map，所以 size 給 0 即可
    //        const ProcessState State = InspectorConf.ObjectInspector.OpenContentEvent.State;
    //        if (IsEventAcceptable(State)) {
    //            InspectorConf.ObjectInspector.OpenContentEvent.Name = Name;
    //            InspectorConf.ObjectInspector.OpenContentEvent.Address = Address;
    //            InspectorConf.ObjectInspector.OpenContentEvent.EditorEnable = EditorEnable;
    //        }
    //        return true;
    //    }
    //    return false;
    //}

    //bool UpdateObjectContentPage(DWORD_PTR Address, std::string Type, std::string Name, bool EditorEnable, bool IsInstance = false, DWORD_PTR InstanceAddress = 0x0, size_t Size = 0)
    //{
    //    const ProcessState State = InspectorConf.InspectorObjectContent.CreatEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        InspectorConf.InspectorObjectContent.CreatEvent.Type = Type;
    //        InspectorConf.InspectorObjectContent.CreatEvent.Name = Name;
    //        InspectorConf.InspectorObjectContent.CreatEvent.Address = Address;
    //        InspectorConf.InspectorObjectContent.CreatEvent.EditorEnable = EditorEnable;
    //        InspectorConf.InspectorObjectContent.CreatEvent.IsInstance = IsInstance;
    //        InspectorConf.InspectorObjectContent.CreatEvent.InstanceAddress = InstanceAddress;
    //        InspectorConf.InspectorObjectContent.CreatEvent.Size = Size;
    //        InspectorConf.InspectorObjectContent.CreatEvent.State = ProcessState::Start;
    //        return true;
    //    }
    //    return false;
    //}

    //bool ObjectInstanceSearch_Event(std::string SearchStr) {
    //    const ProcessState State = InspectorConf.ObjectInstanceSearch.SearchEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        InspectorConf.ObjectInstanceSearch.SearchEvent.SearchStr = SearchStr;
    //        InspectorConf.ObjectInstanceSearch.SearchEvent.State = ProcessState::Start;
    //        return true;
    //    }
    //    return false;
    //}

    //// ============================================================================================================
    //// API
    //void AddAPIObject(DWORD_PTR BasicAddress, DWORD_PTR CurAddress, size_t Offset, int Bit, DWORD_PTR ObjectAddress, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath) {
    //    const ProcessState State = APIConf.AddEvent.State;
    //    if (IsEventAcceptable(State)) {
    //        APIConf.AddEvent.BaseAddress = BasicAddress;
    //        APIConf.AddEvent.Address = CurAddress;
    //        APIConf.AddEvent.Offset = Offset;
    //        APIConf.AddEvent.DepthPath = DepthPath;
    //        APIConf.AddEvent.Bit = Bit;
    //        APIConf.AddEvent.ObjectAddress = ObjectAddress;
    //        APIConf.AddEvent.State = ProcessState::Start;
    //    }
    //}


    
};
