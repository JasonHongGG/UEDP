#pragma once
#include "../GlobalState.h"

namespace EventHandler
{
    inline bool IsEventAcceptable(const ProcessState& State)
    {
        if (State != ProcessState::Start and
            State != ProcessState::Processing and
            State != ProcessState::Retry)
            return true;
        else
            return false;
    }
}