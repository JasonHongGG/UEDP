#pragma once

namespace EventHandler
{
    inline void GetMostUperImportanceObjectVector(int keepCnt) {
        const ProcessState State = ObjectGraphState.GetMostUperImportanceObjectVectorEvent.State;
        if (IsEventAcceptable(State)) {
            ObjectGraphState.GetMostUperImportanceObjectVectorEvent.Cnt = keepCnt;
            ObjectGraphState.GetMostUperImportanceObjectVectorEvent.State = ProcessState::Start;
        }
    }
}