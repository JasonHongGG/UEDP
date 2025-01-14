#pragma once
#include <string>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../System/Memory.h"
#include "Config/ToggleConfig.h"
#include "../State/EventHandler.h"

class GUIUtilsClass
{
public:
    bool IsExpandType(std::string Type);
    void GetDisplayNextState(BasicDumperObject& MemberObject, DWORD_PTR Address, int CurOffset, float Indentation, bool EditorEnable);
    void DrawTreeNavLine(int State, float IndentSize = 0);
    float CursorPosCalcu(float ObjectIndentation, float Indentation, float IndentAccum, bool IsGroupHeaderEle = false, bool IsIndentationNecessary = false, float SetIndentation = -1);
    void GetObjectName(BasicDumperObject& MemberObject, std::string& TargetObjectName, bool EditorEnable);
    // ============================================================================================
    //                                  Style 
    // ============================================================================================

    float GetFrameHeight();
    float GetFontHeight();
    float GetStringWidth(std::string Str);


    void CEItemExport(std::string Name, std::string Type, DWORD_PTR Offset);

    template<typename T>
    bool ValueRenderComponent(std::string UniqueLabel, std::string Type, DWORD_PTR BaseAddress, DWORD_PTR MemberAddress);

    bool ValueRender(std::string UniqueLabel, DWORD_PTR BaseAddress, DWORD_PTR Address, BasicDumperObject& MemberObject);

};
inline GUIUtilsClass GUIUtils = GUIUtilsClass();

bool GUIUtilsClass::IsExpandType(std::string Type)
{
    if (Type.find("StructProperty") != std::string::npos or
        Type.find("ScriptStruct") != std::string::npos or
        Type.find("ClassProperty") != std::string::npos or
        Type.find("ObjectProperty") != std::string::npos or
        Type.find("Class") != std::string::npos or
        Type.find("ArrayProperty") != std::string::npos or
        Type.find("MapProperty") != std::string::npos)
        return true;
    else return false;
}

void GUIUtilsClass::GetDisplayNextState(BasicDumperObject& MemberObject, DWORD_PTR Address, int CurOffset, float Indentation, bool EditorEnable)
{
    if (MemberObject.Type.find("ObjectProperty") != std::string::npos or MemberObject.Type == "Class") {
        DisplayNextState.ShowHeader = true;
        DisplayNextState.NextAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + CurOffset);
        DisplayNextState.NextOffest = 0; //遇到 Object 時歸零 (每個 Object 內的變數 Offset 是各自獨立的)
        DisplayNextState.NextIndentation = Indentation + InspectorConf.InspectorBaseIndentation;
    }
    else if (MemberObject.Type.find("StructProperty") != std::string::npos) {
        DisplayNextState.ShowHeader = true;
        DisplayNextState.NextAddress = Address;
        DisplayNextState.NextOffest = CurOffset;
        DisplayNextState.NextIndentation = Indentation + InspectorConf.InspectorBaseIndentation;
    }
    else if (MemberObject.Type.find("ArrayProperty") != std::string::npos or MemberObject.Type.find("MapProperty") != std::string::npos) {
        DisplayNextState.ShowHeader = false;
        DisplayNextState.NextAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + CurOffset);;
        DisplayNextState.NextOffest = 0;
        DisplayNextState.Size = (EditorEnable ? MemberObject.Size[0] : 1);
        DisplayNextState.NextIndentation = Indentation + InspectorConf.InspectorBaseIndentation;
    }
    else {
        DisplayNextState.ShowHeader = false;
        DisplayNextState.NextAddress = Address;
        DisplayNextState.NextOffest = CurOffset; //累加 Offset (同一個 Object 內的 Offest 會逐漸累加)
        DisplayNextState.NextIndentation = Indentation + InspectorConf.InspectorBaseIndentation;
    }
}

void GUIUtilsClass::DrawTreeNavLine(int State, float IndentSize)
{
    // 開關
    if (InspectorConf.InspectorTreeNavLine == false) return;


    if (State == InspectorConfig::TreeNavLine::Start) {
        InspectorConf.NavLineStartStack.push(ImGui::GetCursorScreenPos());   // 用 stack 去紀錄 Start Pos
        InspectorConf.NavLineEndStack.push(ImGui::GetCursorScreenPos());
    }
    else if (State == InspectorConfig::TreeNavLine::Update) {
        // X 不變、Y 更新
        float ItemMidPoint = (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) / 2;
        ImVec2 UpdateVec = { InspectorConf.NavLineEndStack.top().x, ItemMidPoint };
        InspectorConf.NavLineEndStack.pop();
        InspectorConf.NavLineEndStack.push(UpdateVec);

        // 每次 Update 的時候就是新 Item 出現時 => 畫分支
        // RelativeDistanct > 0 表示 Cursor 大於 Offset 的位置
        // RelativeDistanct 是相對於的位置，目前是以 Offset 的位置為準
        ImVec2 RelativeDistanct = { InspectorConf.RelativeDistanctWithOffset, 0 };
        ImVec2 Start = UpdateVec - RelativeDistanct;
        ImVec2 End = UpdateVec - RelativeDistanct;
        float NavLineOffset = UpdateVec.x - RelativeDistanct.x - ImGui::GetWindowPos().x;
        IndentSize -= NavLineOffset;    // 減掉線本身的偏移，才時此 branch 真正的長度
        End.x += IndentSize - (InspectorConf.InspectorTreeNavLineOffset / 2) + InspectorConf.InspectorTreeNavBranchLineSize;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(Start, End, ImGui::ColorConvertFloat4ToU32(Color::TreeNavLineColor));
    }
    else if (State == InspectorConfig::TreeNavLine::Draw) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 RelativeDistanct = { InspectorConf.RelativeDistanctWithOffset, 0 };
        ImVec2 Start = InspectorConf.NavLineStartStack.top() - RelativeDistanct; // pop 出 Start Pos
        ImVec2 End = InspectorConf.NavLineEndStack.top() - RelativeDistanct;
        drawList->AddLine(Start, End, ImGui::ColorConvertFloat4ToU32(Color::TreeNavLineColor));
        InspectorConf.NavLineStartStack.pop();
        InspectorConf.NavLineEndStack.pop();
    }
}

float GUIUtilsClass::CursorPosCalcu(float ObjectIndentation, float Indentation, float IndentAccum, bool IsGroupHeaderEle, bool IsIndentationNecessary, float SetIndentation)
{
    // ObjectIndentation :  當前物件的 Indentation 
    // Indentation : 目前為止，到上一回合已經累積的 Indentation
    // IndentAccum : 此回合累積(考量物件字串寬度)的 Indentation
    // IsGroupHeaderEle : 該 ROW 的錨點元素用來定位的
    // IsIndentationNecessary : 該 ROW 的開頭元素，讓不管結構怎麼調樹狀的縮排都一定要有
    if (!IsGroupHeaderEle and !InspectorConf.IndentationIndependent)    // 是 relative 且不是 group 的第一個元素，則跟著前面元素尾巴依序接下去
        return SetIndentation == -1 ? ImGui::GetCursorPosX() : SetIndentation;

    float UseObjectIndentation = (InspectorConf.IndentationIndependent or IsGroupHeaderEle) ? ObjectIndentation : 0;    // 是否使用 Indentation      //如果是 IsGroupHeaderEle 則必須得用 ObjectIndentation
    float CurrentIndentation = InspectorConf.IndentationAccumulate ? Indentation : IndentAccum;     // Indentation 是否考量到每個物件字串本身的寬度

    if ((InspectorConf.IndentationIndependent or (InspectorConf.AlignGroupObjectIndentation and IsGroupHeaderEle)) and !IsIndentationNecessary)   // 獨立使用 ObjectIndentation// 使用 ObjectIndentation 且考慮縮排
        return UseObjectIndentation;            // 有沒有數值取決於是否 UseObjectIndentation
    else
        return UseObjectIndentation + CurrentIndentation;   //考慮到縮排、縮排是否考慮字串長度取決於 IndentationAccumulate、 是某使用 ObjectIndentation 取決於 UseObjectIndentation 
}

void GUIUtilsClass::GetObjectName(BasicDumperObject& MemberObject, std::string& TargetObjectName, bool EditorEnable)
{
    if (MemberObject.Type.find("StructProperty") != std::string::npos or
        MemberObject.Type.find("ObjectProperty") != std::string::npos or
        MemberObject.Type.find("ClassProperty") != std::string::npos)
        TargetObjectName = (MemberObject.Index != -1 ? std::to_string(MemberObject.Index) + " " : "") +
        MemberObject.SubType[0].Name;

    else if (MemberObject.Type.find("ArrayProperty") != std::string::npos)
        TargetObjectName = (MemberObject.Index != -1 ? std::to_string(MemberObject.Index) + " " : "") + "[Array] " +
        ((EditorEnable and MemberObject.Size[0] < 200) ? "[" + std::to_string(MemberObject.Size[0]) + "]" + "[" + std::to_string(MemberObject.Size[1]) + "]" : "") +
        (MemberObject.SubType[0].Type.find("Property") != std::string::npos ? MemberObject.SubType[0].Name : MemberObject.SubType[0].Type);

    else if (MemberObject.Type.find("MapProperty") != std::string::npos)
        TargetObjectName = (MemberObject.Index != -1 ? std::to_string(MemberObject.Index) + " " : "") + "[Map] " +
        ((EditorEnable and MemberObject.Size[0] < 200) ? "[" + std::to_string(MemberObject.Size[0]) + "]" + "[" + std::to_string(MemberObject.Size[1]) + "]" : "") +
        (MemberObject.SubType[0].Type.find("Property") != std::string::npos ? MemberObject.SubType[0].Name : MemberObject.SubType[0].Type);

    else if (MemberObject.Type.find("ScriptStruct") != std::string::npos)
        TargetObjectName = MemberObject.Name;

    else if (MemberObject.Type.find("EnumProperty") != std::string::npos)
        TargetObjectName = (MemberObject.Index != -1 ? std::to_string(MemberObject.Index) + " " : "") + "[Enum] " +
        MemberObject.SubType[0].Name;
}

float GUIUtilsClass::GetFrameHeight()
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float FrameHeight = ImGui::CalcTextSize("Get GUObject", NULL, true).y + style.FramePadding.y * 2.0f;
    return FrameHeight;
}

float GUIUtilsClass::GetFontHeight()
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float FontHeight = ImGui::CalcTextSize("Get GUObject", NULL, true).y;
    return FontHeight;
}

float GUIUtilsClass::GetStringWidth(std::string Str)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float StringWidth = ImGui::CalcTextSize(Str.c_str(), NULL, true).x;
    return StringWidth;
}

void GUIUtilsClass::CEItemExport(std::string Name, std::string Type, DWORD_PTR Offset) {
    std::string CheatEngineItemFormat =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "<CheatTable>"
        "<CheatEntries>"
        "<CheatEntry>"
        "<ID>5</ID>"
        "<Description>\"" + Name + "\"</Description>"
        "<ShowAsSigned>0</ShowAsSigned>"
        "<VariableType>" + Type + "</VariableType>"
        "<Address> +" + Utils.HexToString(Offset) + "</Address >"
        "</CheatEntry>"
        "</CheatEntries>"
        "</CheatTable>";

    ImGui::SameLine();
    ImGui::CopyBtn(Name + "_CheatEngineExport", CheatEngineItemFormat, -1, Style::TreeNodePadding);
}

template<typename T>
bool GUIUtilsClass::ValueRenderComponent(std::string UniqueLabel, std::string Type, DWORD_PTR BaseAddress, DWORD_PTR MemberAddress)
{
    T Value = MemMgr.MemReader.ReadMem<T>(MemberAddress);
    if (!Value) return false;

    ImGui::PushItemWidth(50);
    if (Type == "Int") {
        EditorMemMgr.MapInitIfNotExist<int>(EditorMemMgr.ObjectStroageData[BaseAddress].IntType, UniqueLabel, 0);
        EditorMemMgr.ObjectStroageData[BaseAddress].IntType[UniqueLabel] = Value;
        ImGui::DragInt((UniqueLabel).c_str(), (int*)&EditorMemMgr.ObjectStroageData[BaseAddress].IntType[UniqueLabel]);
        if (ImGui::IsItemEdited()) {
            MemMgr.MemWriter.WriteMem<T>(MemberAddress, EditorMemMgr.ObjectStroageData[BaseAddress].IntType[UniqueLabel]);
        }
    }
    else if (Type == "Float") {
        EditorMemMgr.MapInitIfNotExist<float>(EditorMemMgr.ObjectStroageData[BaseAddress].FloatType, UniqueLabel, 0);
        EditorMemMgr.ObjectStroageData[BaseAddress].FloatType[UniqueLabel] = Value;
        ImGui::DragFloat((UniqueLabel).c_str(), (float*)&EditorMemMgr.ObjectStroageData[BaseAddress].FloatType[UniqueLabel]);
        if (ImGui::IsItemEdited()) {
            MemMgr.MemWriter.WriteMem<T>(MemberAddress, EditorMemMgr.ObjectStroageData[BaseAddress].FloatType[UniqueLabel]);
        }
    }
    else if (Type == "Name") {
        EditorMemMgr.MapInitIfNotExist<std::string, int32_t>(EditorMemMgr.ObjectStroageData[BaseAddress].NameType, Value, "");
        if (EditorMemMgr.ObjectStroageData[BaseAddress].NameType[Value].empty() and Value != 0)
            EventHandler::EditorGetFName(BaseAddress, Value);
        else {
            ImGui::TextColored(Color::White, Value != 0 ? EditorMemMgr.ObjectStroageData[BaseAddress].NameType[Value].c_str() : "None");
        }
    }

    ImGui::PopItemWidth();
    return true;
}

bool GUIUtilsClass::ValueRender(std::string UniqueLabel, DWORD_PTR BaseAddress, DWORD_PTR Address, BasicDumperObject& MemberObject)
{
    // Input Para
    std::string Name = MemberObject.Name;
    std::string Type = MemberObject.Type;
    size_t Offset = MemberObject.Offset;
    std::vector<std::pair<std::string, size_t>>& EnumList = MemberObject.Enum;
    int BitIdx = MemberObject.Bit;

    // Int
    if (Type.find("Int64Property") != std::string::npos) {         //Drag
        ValueRenderComponent<int64_t>("##Int64_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "8 Bytes", Offset);
    }
    else if (Type.find("IntProperty") != std::string::npos or Type.find("Int32Property") != std::string::npos) {         //Drag
        ValueRenderComponent<int32_t>("##Int32_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "4 Bytes", Offset);
    }
    else if (Type.find("Int16Property") != std::string::npos) {         //Drag
        ValueRenderComponent<int16_t>("##Int16_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "2 Bytes", Offset);
    }
    else if (Type.find("ByteProperty") != std::string::npos or Type.find("Int8Property") != std::string::npos) {         //Drag 
        if (MemberObject.Enum.size() > 0) {
            BasicDumperObject TempObj = MemberObject;
            TempObj.Type = "EnumProperty";  // 導向 Enum
            ValueRender(UniqueLabel, BaseAddress, Address, TempObj);
        }
        else {
            ValueRenderComponent<int8_t>("##Int8_Property" + UniqueLabel, "Int", BaseAddress, Address);
            CEItemExport(Name, "Byte", Offset);
        }
    }
    // Uint
    else if (Type.find("UInt64Property") != std::string::npos) {        //Drag
        ValueRenderComponent<uint64_t>("##Uint64_t_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "8 Bytes", Offset);
    }
    else if (Type.find("UInt32Property") != std::string::npos) {        //Drag
        ValueRenderComponent<uint32_t>("##Uint32_t_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "4 Bytes", Offset);
    }
    else if (Type.find("UInt16Property") != std::string::npos) {        //Drag
        ValueRenderComponent<uint16_t>("##Uint16_t_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "2 Bytes", Offset);
    }
    else if (Type.find("UInt8Property") != std::string::npos) {          //Drag
        ValueRenderComponent<uint8_t>("##Uint8_t_Property" + UniqueLabel, "Int", BaseAddress, Address);
        CEItemExport(Name, "Byte", Offset);
    }
    // Float
    else if (Type.find("FloatProperty") != std::string::npos) {          //Drag
        ValueRenderComponent<float>("##Float_Property" + UniqueLabel, "Float", BaseAddress, Address);
        CEItemExport(Name, "Float", Offset);
    }
    // Double
    else if (Type.find("DoubleProperty") != std::string::npos) {          //Drag
        ValueRenderComponent<double>("##Double_Property" + UniqueLabel, "Float", BaseAddress, Address);
        CEItemExport(Name, "Double", Offset);
    }
    // Bool
    else if (Type.find("BoolProperty") != std::string::npos) {          //Drag
        uint8_t Value = MemMgr.MemReader.ReadMem<uint8_t>(Address);
        if (!Value) return false;
        bool IsBitActivate = Value & Utils.BitMaskTable.at(BitIdx);
        EditorMemMgr.ObjectStroageData[BaseAddress].BoolType[UniqueLabel] = IsBitActivate;
        if (ImGui::Toggle(("##Bool_Property" + UniqueLabel).c_str(), &EditorMemMgr.ObjectStroageData[BaseAddress].BoolType[UniqueLabel], ToggleConf)) {
            if (IsBitActivate)
                Value = Value & ~Utils.BitMaskTable.at(BitIdx);     // Delete
            else
                Value = Value | Utils.BitMaskTable.at(BitIdx);      // Add

            MemMgr.MemWriter.WriteMem<uint8_t>(Address, Value);
        }
    }
    //Enum
    else if (Type.find("EnumProperty") != std::string::npos) {          //Drag
        uint8_t Value = MemMgr.MemReader.ReadMem<uint8_t>(Address);
        if (!Value) return false;
        
        if (!EnumList.empty()) {
            // 初始化
            EditorMemMgr.MapInitIfNotExist<std::pair<std::string, int>>(EditorMemMgr.ObjectStroageData[BaseAddress].EnumType, UniqueLabel, { "None" , 0 });
            // 設定
            if (EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].first == "None") {
                for (int i = 0; i < EnumList.size(); ++i) {
                    if (EnumList[i].second == Value) EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel] = { EnumList[i].first, EnumList[i].second };
                }
            }
            // 選單
            ImGui::PushItemWidth(100);
            bool IsValueChange = false;
            std::string ShowOption = EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].first;
            if (ImGui::BeginCombo(("##Enum_Property" + UniqueLabel).c_str(), ShowOption.substr((ShowOption.find("::") != std::string::npos ? ShowOption.find("::") + 2 : 0)).c_str())) {
                for (int i = 0; i < EnumList.size(); ++i) {
                    const bool IsSelected = (EnumList[i].second == Value);

                    if (ImGui::Selectable((EnumList[i].first.substr((EnumList[i].first.find("::") != std::string::npos ? EnumList[i].first.find("::") + 2 : 0))).c_str(), IsSelected)) {      // + 2 是為了跳過 "::"
                        if (EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].second != EnumList[i].second) IsValueChange = true;
                        EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel] = { EnumList[i].first, EnumList[i].second };
                    }

                    if (IsSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (IsValueChange) {
                MemMgr.MemWriter.WriteMem<uint8_t>(Address, (uint8_t)EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].second);
            }
            ImGui::PopItemWidth();
        }
        else {
            ImGui::TextColored(Color::ValueColor, "EnumList Is Empty");
        }
    }
    // Name
    else if (Type.find("NameProperty") != std::string::npos) {
        ValueRenderComponent<int>("##Name_Property" + UniqueLabel, "Name", BaseAddress, Address);
    }
    else if (Type.find("StrProperty") != std::string::npos) {
        static DWORD_PTR StrAddress;
        static BYTE buffer[60];
        static std::wstring output;
        StrAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address);
        if (!StrAddress) return false;
        MemMgr.MemReader.ReadBytes(StrAddress, buffer, 50);     // 字串最常以讀取 50
        buffer[58] = '\0'; buffer[59] = '\0';
        output = std::wstring((wchar_t*)buffer);
        ImGui::TextColored(Color::White, (!output.empty()) ? Utils.UnicodeToUTF8(output.c_str()).c_str() : "None");
    }
    else {
        DWORD_PTR Value = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address);
        if (!Value) return false;
        // => Address
        if (
            Type.find("MapProperty") != std::string::npos or
            Type.find("ArrayProperty") != std::string::npos or
            Type.find("StructProperty") != std::string::npos or
            Type.find("ClassProperty") != std::string::npos or
            Type.find("ClassPtrProperty") != std::string::npos or
            Type.find("ObjectProperty") != std::string::npos
            )
        {
            //Btn
            ImGui::CopyBtn(UniqueLabel + "_EditorEnable", Utils.HexToString(Value), -1, Style::TreeNodePadding);
            ImGui::SameLine(0, 0); ImGui::TextColored(Color::ValueColor, Utils.HexToString(Value).c_str());
        }
        else {
            ImGui::TextColored(Color::ValueColor, Utils.HexToString(Value).c_str());
        }
    }

    return true;


    //else if (
    //    Type.find("StrProperty") != std::string::npos or
    //    Type.find("TextProperty") != std::string::npos
    //    )
    //{

    //}

    // 可能未來會支援
    //ObjectPropertyBase
    //NumericProperty
    //PropertyWrapper
    //DelegateProperty
    //WeakObjectProperty
    //FieldPathProperty
}