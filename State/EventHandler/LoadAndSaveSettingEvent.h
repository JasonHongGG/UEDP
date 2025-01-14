#pragma once

namespace EventHandler
{
    inline void SaveSetting() {
        const ProcessState State = LoadAndSaveSettingState.SaveEvent.State;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingState.SaveEvent.State = ProcessState::Start;
        }
    }

    inline void ReadSetting() {
        const ProcessState State = LoadAndSaveSettingState.ReadEvent.State;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingState.ReadEvent.State = ProcessState::Start;
        }
    }

    inline void LoadSetting(const std::string& CurTime) {
        const ProcessState State = LoadAndSaveSettingState.LoadEvent.State;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingState.CurTime = CurTime;
            LoadAndSaveSettingState.LoadEvent.State = ProcessState::Start;
        }
    }

    inline bool DeleteSetting(const std::string& CurTime) {
        const ProcessState State = LoadAndSaveSettingState.DeleteEvent.State;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingState.CurTime = CurTime;
            LoadAndSaveSettingState.DeleteEvent.State = ProcessState::Start;
            return true;
        }
        return false;
    }
}