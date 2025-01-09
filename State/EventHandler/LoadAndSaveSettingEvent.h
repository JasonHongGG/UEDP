#pragma once

namespace EventHandler
{
    inline void SaveSetting() {
        const ProcessState State = LoadAndSaveSettingStateMgr.SaveEvent;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingStateMgr.SaveEvent = ProcessState::Start;
        }
    }

    inline void ReadSetting() {
        const ProcessState State = LoadAndSaveSettingStateMgr.ReadEvent;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingStateMgr.ReadEvent = ProcessState::Start;
        }
    }

    inline void LoadSetting(const std::string& CurTime) {
        const ProcessState State = LoadAndSaveSettingStateMgr.LoadEvent;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingStateMgr.CurTime = CurTime;
            LoadAndSaveSettingStateMgr.LoadEvent = ProcessState::Start;
        }
    }

    inline bool DeleteSetting(const std::string& CurTime) {
        const ProcessState State = LoadAndSaveSettingStateMgr.DeleteEvent;
        if (IsEventAcceptable(State)) {
            LoadAndSaveSettingStateMgr.CurTime = CurTime;
            LoadAndSaveSettingStateMgr.DeleteEvent = ProcessState::Start;
            return true;
        }
        return false;
    }
}