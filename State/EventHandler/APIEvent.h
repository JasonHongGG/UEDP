#pragma once


namespace EventHandler
{
    inline void AddAPIObject(DWORD_PTR BasicAddress, DWORD_PTR CurAddress, size_t Offset, int Bit, DWORD_PTR ObjectAddress, std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath) {
        const ProcessState State = APIState.AddEvent.State;
        if (IsEventAcceptable(State)) {
            APIState.AddEvent.BaseAddress = BasicAddress;
            APIState.AddEvent.Address = CurAddress;
            APIState.AddEvent.Offset = Offset;
            APIState.AddEvent.DepthPath = DepthPath;
            APIState.AddEvent.Bit = Bit;
            APIState.AddEvent.ObjectAddress = ObjectAddress;
            APIState.AddEvent.State = ProcessState::Start;
        }
    }

	inline void OpenAPIFile(std::string FilePath) {
		const ProcessState State = APIState.OpenFileEvent.State;
		if (IsEventAcceptable(State)) {
			APIState.OpenFileEvent.FilePath = FilePath;
			APIState.OpenFileEvent.State = ProcessState::Start;
		}
	}

	inline void SaveAPIFile(std::string FilePath) {
		const ProcessState State = APIState.SaveFileEvent.State;
		if (IsEventAcceptable(State)) {
			APIState.SaveFileEvent.FilePath = FilePath;
			APIState.SaveFileEvent.State = ProcessState::Start;
		}
	}

	inline void UpdateAPIObject() {
		const ProcessState State = APIState.UpdateEvent.State;
		if (IsEventAcceptable(State)) {
			APIState.UpdateEvent.State = ProcessState::Start;
		}
	}
}
