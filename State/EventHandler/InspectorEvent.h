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
}