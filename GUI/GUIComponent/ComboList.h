#pragma once
#include <functional>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "../../utils/utils.h"

class MyComboList
{
public:
    using CallbackType = std::function<void(int)>;

private:
    std::string UniqueName;
    int& Idx;
    std::vector<std::string>& List;
    ImGuiComboFlags flags;
    CallbackType callback;

public:
    MyComboList(std::string uniqueName, int& idx, std::vector<std::string>& list, ImGuiComboFlags flags = 0)
        : UniqueName(uniqueName), Idx(idx), List(list), flags(flags) {}

    MyComboList& SetCallBack(CallbackType cb)
    {
        callback = cb;
        return *this;
    }

    void Show()
    {
        if (ImGui::BeginCombo(("##TypeSelector_" + UniqueName).c_str(), List[Idx].c_str(), flags)) {
            for (int i = 0; i < List.size(); ++i) {
                const bool IsSelected = (Idx == i);

                if (ImGui::Selectable(List[i].c_str(), IsSelected, 0, ImVec2(0, 0))) {
                    Idx = i;
                    if (callback) {
                        callback(i);
                    }
                }

                if (IsSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
};

