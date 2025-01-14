#pragma once
#include <string>
#include <vector>
#include "../GUI/Config/NotificationConfig.h"
#include "GlobalState.h"
#include "GUIState.h"

#include "EventHandler/Utils.h"
#include "EventHandler/LoadAndSaveSettingEvent.h"
#include "EventHandler/ConsoleEvent.h"
#include "EventHandler/PackageViewerEvent.h"
#include "EventHandler/InspectorEvent.h"
#include "EventHandler/APIEvent.h"




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
};
