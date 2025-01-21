#pragma once

namespace EventHandler
{
    inline void GetMostUperObjectVector(int keepCnt) {
        const ProcessState State = ObjectGraphState.GetMostUperObjectVectorEvent.State;
        if (IsEventAcceptable(State)) {
            ObjectGraphState.GetMostUperObjectVectorEvent.Cnt = keepCnt;
            ObjectGraphState.GetMostUperObjectVectorEvent.State = ProcessState::Start;
        }
    }
}