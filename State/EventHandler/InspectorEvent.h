#pragma once

namespace EventHandler
{
    inline void EditorGetFName(DWORD_PTR Address, int FNameID)
    {
        const ProcessState State = InspectorState.GetFNameEvent.State;
        if (IsEventAcceptable(State)) {
            InspectorState.GetFNameEvent.Address = Address;
            InspectorState.GetFNameEvent.FNameID = FNameID;
            InspectorState.GetFNameEvent.State = ProcessState::Start;
        }
    }

    inline void AddInspector(DWORD_PTR Address)
    {
        const ProcessState State = InspectorState.AddInspectorEvent.State;
        if (IsEventAcceptable(State)) {
            InspectorState.AddInspectorEvent.Address = Address;
            InspectorState.AddInspectorEvent.State = ProcessState::Start;
        }
    }

    inline bool UpdateObjectContentPage(DWORD_PTR Address, std::string Type, std::string Name, bool EditorEnable, bool IsInstance = false, DWORD_PTR InstanceAddress = 0x0, size_t Size = 0)
    {
        const ProcessState State = InspectorState.ObjectContentCreatEvent.State;
        if (IsEventAcceptable(State)) {
            InspectorState.ObjectContentCreatEvent.Type = Type;
            InspectorState.ObjectContentCreatEvent.Name = Name;
            InspectorState.ObjectContentCreatEvent.Address = Address;
            InspectorState.ObjectContentCreatEvent.EditorEnable = EditorEnable;
            InspectorState.ObjectContentCreatEvent.IsInstance = IsInstance;
            InspectorState.ObjectContentCreatEvent.InstanceAddress = InstanceAddress;
            InspectorState.ObjectContentCreatEvent.Size = Size;
            InspectorState.ObjectContentCreatEvent.State = ProcessState::Start;
            return true;
        }
        return false;
    }

    inline bool OpenObjectContentPage(DWORD_PTR Address, std::string Type, std::string Name, bool EditorEnable, bool IsInstance)
    {
        if (UpdateObjectContentPage(Address, Type, Name, EditorEnable, IsInstance, Address, 0)) {         // 創建新 Page 時不可能是 array、map，所以 size 給 0 即可
            const ProcessState State = InspectorState.ObjectContentOpenEvent.State;
            if (IsEventAcceptable(State)) {
                InspectorState.ObjectContentOpenEvent.Name = Name;
                InspectorState.ObjectContentOpenEvent.Address = Address;
                InspectorState.ObjectContentOpenEvent.EditorEnable = EditorEnable;
            }
            return true;
        }
        return false;
    }

    inline bool ObjectInstanceSearch(std::string SearchStr) 
    {
        const ProcessState State = InspectorState.ObjectInstanceSearchEvent.State;
        if (IsEventAcceptable(State)) {
            InspectorState.ObjectInstanceSearchEvent.SearchStr = SearchStr;
            InspectorState.ObjectInstanceSearchEvent.State = ProcessState::Start;
            return true;
        }
        return false;
    }
}