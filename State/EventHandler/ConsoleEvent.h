#pragma once

namespace EventHandler
{
    inline void GetFName()
    {
        const ProcessState State = MainConsoleState.FNameInfoGetEvent;
        if (IsEventAcceptable(State)) {
            MainConsoleState.FNameInfoGetEvent = ProcessState::Start;
        }
    }

    inline void GetGUObject()
    {
        const ProcessState State = MainConsoleState.GUObjectInfoGetEvent;
        if (IsEventAcceptable(State)) {
            MainConsoleState.GUObjectInfoGetEvent = ProcessState::Start;
        }
    }

    inline void AddToObjectTable()
    {
        const ProcessState State = MainConsoleState.GUObjectInfoAddEvent;
        if (IsEventAcceptable(State)) {
            MainConsoleState.GUObjectInfoAddEvent = ProcessState::Start;
        }
    }

    inline void ParseObject()
    {
        const ProcessState State = MainConsoleState.GUObjectInfoParseEvent;
        if (IsEventAcceptable(State)) {
            MainConsoleState.GUObjectInfoParseEvent = ProcessState::Start;
        }
    }

    inline void UpdateDumperObjectMap(DWORD_PTR Address, int Index)
    {
        const ProcessState State = MainConsoleState.GUObjectInfoExpandEvent.State;
        if (IsEventAcceptable(State)) {
            MainConsoleState.GUObjectInfoExpandEvent.Index = Index;
            MainConsoleState.GUObjectInfoExpandEvent.Address = Address;
            MainConsoleState.GUObjectInfoExpandEvent.State = ProcessState::Start;
        }
    }
}