#pragma once
#include <functional>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "../../utils/utils.h"
#include "../../font/IconsFontAwesome6.h"

class MyInputBarBtn
{
public:
    using CallbackType = std::function<void(int)>;

    enum StateStruct {
        Close = 0,
        Open = 1,
    };

private:
    std::string UniqueName;
    std::string HintStr;
    std::string& Target;
    int& TargetIdx;
    int TargetMaxIdx;
    bool& SearchFocus;
    StateStruct& State;
    float& StateTimer;   //I
    float StateTimerMax;
    float StateTimerMin;
    float TimerSpeed = 0.5;
    ImVec2 Pos;

    ImFont* Font = nullptr;
    CallbackType callback;

public:
    MyInputBarBtn(std::string uniqueName, std::string hint, float& v, float vMin, float vMax, std::string& tar, int& tarIdx, int tarMaxIdx, bool& searchFocus, StateStruct& state)
        : UniqueName(uniqueName), HintStr(hint), StateTimer(v), StateTimerMin(vMin), StateTimerMax(vMax), Target(tar), TargetIdx(tarIdx), TargetMaxIdx(tarMaxIdx), SearchFocus(searchFocus), State(state) {}

    MyInputBarBtn& SetFont(ImFont* Icon)
    {
        Font = Icon;
        return *this;
    }

    MyInputBarBtn& SetPos(ImVec2 pos)
    {
        Pos = pos;
        return *this;
    }

    MyInputBarBtn& SetSpeed(float value)
    {
        TimerSpeed = value;
        return *this;
    }

    MyInputBarBtn& SetCallBack(CallbackType cb)
    {
        callback = cb;
        return *this;
    }

    void InputTextWithHintWithWidth(const char* label, const char* hint, std::string& text, float Width = 0, bool forceSet = false)
    {
        char TextBuf[128];
        strcpy_s(TextBuf, sizeof(TextBuf), text.c_str());
        if (Width > 0 or forceSet) ImGui::PushItemWidth(Width);
        ImGui::InputTextWithHint(label, hint, TextBuf, IM_ARRAYSIZE(TextBuf));
        if (Width > 0 or forceSet) ImGui::PopItemWidth();
        text = std::string(TextBuf);    // return back
    }

    void Show()
    {
        Pos.x -= (StateTimer - StateTimerMin + (StateTimer == StateTimerMax ? 68 : 0)) ;
        ImGui::SetCursorPos(Pos);
        std::string Title = "##InputBarBtn" + UniqueName;
        if (Font) Title = (State == StateStruct::Close ? ICON_FA_CHEVRON_LEFT : ICON_FA_CHEVRON_RIGHT) + Title;
        if (Font) ImGui::PushFont(Font);
        if (ImGui::Button(Title.c_str(), ImVec2(30, 0))) {
            if (State == StateStruct::Close)
                State = StateStruct::Open;
            else
                State = StateStruct::Close;
        }
        if (Font) ImGui::PopFont();

        if ((State == StateStruct::Open) or (StateTimer != StateTimerMin)) {  // when open or close but timer no over
            ImGui::SameLine(0, 4);
            InputTextWithHintWithWidth(("##InputBar" + UniqueName).c_str(), HintStr.c_str(), Target, StateTimer);

            if(StateTimer == StateTimerMax){
                if (Font) ImGui::PushFont(Font);
                ImGui::SameLine(0, 4);
                if (ImGui::Button((ICON_FA_CHEVRON_DOWN "##NextBtn" + UniqueName).c_str(), ImVec2(30, 0))) {
                    if (TargetIdx < TargetMaxIdx) {
                        TargetIdx++;
                        SearchFocus = true;
                    }
                }

                ImGui::SameLine(0, 4);
                if (ImGui::Button((ICON_FA_CHEVRON_UP "##PreBtn" + UniqueName).c_str(), ImVec2(30, 0))) {
                    if (TargetIdx > 1) {  // ³Ì¤p¨ì 1
                        TargetIdx--;
                        SearchFocus = true;
                    }
                }
                if (Font) ImGui::PopFont();
            }
        }

        // Timer Update
        if (State == StateStruct::Open) {
            if (StateTimer < StateTimerMax)
                StateTimer += TimerSpeed;

            if (StateTimer > StateTimerMax)
                StateTimer = StateTimerMax;
        }
        else {
            if (StateTimer > StateTimerMin)
                StateTimer -= TimerSpeed;

            if (StateTimer < StateTimerMin)
                StateTimer = StateTimerMin;
        }



    }
};
