#pragma once
#include <functional>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "../../utils/utils.h"

// 定義 ContainerTraits
template<typename T>
struct ContainerTraits;

template<typename T> // Specialization for std::vector
struct ContainerTraits<std::vector<T>> {
    using ValueType = T;
    using ValuePairType = std::false_type;
    using IsMap = std::false_type;
};

template<typename K, typename V> // Specialization for std::map
struct ContainerTraits<std::map<K, V>> {
    using KeyType = K;
    using ValueType = V;
    using ValuePairType = std::pair<const K, V>;
    using IsMap = std::true_type;
};

template<typename Container>
class MyListBox
{
public:
    using Traits = ContainerTraits<Container>;
    using IsMap = typename Traits::IsMap;

    using VectorCallbackType = std::function<void(int)>;
    using MapCallbackType = std::function<void(int, typename Traits::ValuePairType&)>;

    using CallbackType = std::conditional_t<
        IsMap::value,
        MapCallbackType,
        VectorCallbackType
    >;

    using NamePreProcCallbackType = std::function<void(std::string&)>;

private:
    std::string UniqueName;
    int& Idx;
    Container& List;
    const std::string& FilterStr;
    const ImVec2& Size;

    CallbackType SelectCallback;
    CallbackType PostCallback;
    bool UseCustomFont = false;
    ImFont* Font = nullptr;
    bool UseNamePreProc = false;
    NamePreProcCallbackType NamePreProcCallback;
    ImGuiSelectableFlags Flags = 0;

public:
    MyListBox(std::string uniqueName, int& idx, Container& list, const ImVec2& size, const std::string& filterStr = "", ImGuiSelectableFlags flags = 0)
        : UniqueName(uniqueName), Idx(idx), List(list), Size(size), FilterStr(filterStr), Flags(flags) {}

    MyListBox& SetNamePreProcCallBack(NamePreProcCallbackType cb)
    {
        UseNamePreProc = true;
        NamePreProcCallback = cb;
        return *this;
    }

    MyListBox& SetSelectCallBack(CallbackType cb)
    {
        SelectCallback = cb;
        return *this;
    }

    MyListBox& SetPostCallBack(CallbackType cb)
    {
        PostCallback = cb;
        return *this;
    }

    MyListBox& SetFont(ImFont* font)
    {
        UseCustomFont = true;
        Font = font;
        return *this;
    }

    void Show()
    {
        if (ImGui::BeginListBox(("##MyListBox_" + UniqueName).c_str(), Size)) {
            ShowImpl(IsMap{});
            ImGui::EndListBox();
        }
    }

private:
    template <typename T = typename Traits::ValueType>
    auto GetDisplayText(const T& item) -> const std::string&  //decltype(item.Name)
    {
        return item.Name;
    }

    template <typename T = typename Traits::ValueType>
    auto GetDisplayText(const std::string& item) -> const std::string&
    {
        return item;
    }

    void ShowImpl(std::false_type)
    {
        // Implementation for std::vector
        for (int i = 0; i < List.size(); ++i) {
            ImGui::PushID(i);

            std::string DisplayText = GetDisplayText(List[i]);
            if (UseNamePreProc) NamePreProcCallback(DisplayText);

            //Filter 是否啟動
            if (FilterStr.size() != 0)
                if (Utils.rStringToLower(DisplayText).find(Utils.rStringToLower(FilterStr)) == std::string::npos) {
                    ImGui::PopID();
                    continue;
                }

            //選擇的清單顯示
            if (UseCustomFont) ImGui::PushFont(Font);
            const bool IsSelected = (Idx == i);
            if (ImGui::Selectable(DisplayText.c_str(), IsSelected, Flags)) {
                Idx = i;
                if (SelectCallback) SelectCallback(i);
            }
            if (IsSelected) {
                ImGui::SetItemDefaultFocus();
            }
            if (UseCustomFont) ImGui::PopFont();

            // 其餘動作
            if (PostCallback) PostCallback(i);

            ImGui::PopID();
        }
    }


    // std::pair
    template<typename T>
    struct is_pair : std::false_type {};

    template<typename T1, typename T2>
    struct is_pair<std::pair<T1, T2>> : std::true_type {};

    // 特化版本：item.second.first
    template <typename T>
    auto GetMapDisplayText(const T& item) -> typename std::enable_if<is_pair<typename T::second_type>::value, const std::string&>::type
    {
        return item.second.first;
    }

    // 通用版本：item.first
    template <typename T>
    auto GetMapDisplayText(const T& item) -> typename std::enable_if<!is_pair<typename T::second_type>::value, const std::string&>::type
    {
        return item.first;
    }

    void ShowImpl(std::true_type)
    {
        // Implementation for std::map
        int i = 0;
        for (auto& pairObj : List) {
            ImGui::PushID(i);

            std::string DisplayText = GetMapDisplayText(pairObj);
            if (UseNamePreProc) NamePreProcCallback(DisplayText);

            if (FilterStr.size() != 0)
                if (Utils.rStringToLower(DisplayText).find(Utils.rStringToLower(FilterStr)) == std::string::npos) {
                    ImGui::PopID();
                    continue;
                }
                    

            if (UseCustomFont) ImGui::PushFont(Font);
            const bool IsSelected = (Idx == i);
            if (ImGui::Selectable(DisplayText.c_str(), IsSelected, Flags)) {
                Idx = i;
                if (SelectCallback) SelectCallback(i, pairObj);
            }
            if (IsSelected) {
                ImGui::SetItemDefaultFocus();
            }
            if (UseCustomFont) ImGui::PopFont();

            if (PostCallback) PostCallback(i, pairObj);

            i++;

            ImGui::PopID();
        }
    }
};