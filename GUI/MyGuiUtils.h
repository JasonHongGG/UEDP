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
        DisplayNextState.NextOffest = 0; //�J�� Object ���k�s (�C�� Object �����ܼ� Offset �O�U�ۿW�ߪ�)
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
        DisplayNextState.NextOffest = CurOffset; //�֥[ Offset (�P�@�� Object ���� Offest �|�v���֥[)
        DisplayNextState.NextIndentation = Indentation + InspectorConf.InspectorBaseIndentation;
    }
}

void GUIUtilsClass::DrawTreeNavLine(int State, float IndentSize)
{
    // �}��
    if (InspectorConf.InspectorTreeNavLine == false) return;


    if (State == InspectorConfig::TreeNavLine::Start) {
        InspectorConf.NavLineStartStack.push(ImGui::GetCursorScreenPos());   // �� stack �h���� Start Pos
        InspectorConf.NavLineEndStack.push(ImGui::GetCursorScreenPos());
    }
    else if (State == InspectorConfig::TreeNavLine::Update) {
        // X ���ܡBY ��s
        float ItemMidPoint = (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) / 2;
        ImVec2 UpdateVec = { InspectorConf.NavLineEndStack.top().x, ItemMidPoint };
        InspectorConf.NavLineEndStack.pop();
        InspectorConf.NavLineEndStack.push(UpdateVec);

        // �C�� Update ���ɭԴN�O�s Item �X�{�� => �e����
        // RelativeDistanct > 0 ��� Cursor �j�� Offset ����m
        // RelativeDistanct �O�۹�󪺦�m�A�ثe�O�H Offset ����m����
        ImVec2 RelativeDistanct = { InspectorConf.RelativeDistanctWithOffset, 0 };
        ImVec2 Start = UpdateVec - RelativeDistanct;
        ImVec2 End = UpdateVec - RelativeDistanct;
        float NavLineOffset = UpdateVec.x - RelativeDistanct.x - ImGui::GetWindowPos().x;
        IndentSize -= NavLineOffset;    // ��u�����������A�~�ɦ� branch �u��������
        End.x += IndentSize - (InspectorConf.InspectorTreeNavLineOffset / 2) + InspectorConf.InspectorTreeNavBranchLineSize;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(Start, End, ImGui::ColorConvertFloat4ToU32(Color::TreeNavLineColor));
    }
    else if (State == InspectorConfig::TreeNavLine::Draw) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 RelativeDistanct = { InspectorConf.RelativeDistanctWithOffset, 0 };
        ImVec2 Start = InspectorConf.NavLineStartStack.top() - RelativeDistanct; // pop �X Start Pos
        ImVec2 End = InspectorConf.NavLineEndStack.top() - RelativeDistanct;
        drawList->AddLine(Start, End, ImGui::ColorConvertFloat4ToU32(Color::TreeNavLineColor));
        InspectorConf.NavLineStartStack.pop();
        InspectorConf.NavLineEndStack.pop();
    }
}

float GUIUtilsClass::CursorPosCalcu(float ObjectIndentation, float Indentation, float IndentAccum, bool IsGroupHeaderEle, bool IsIndentationNecessary, float SetIndentation)
{
    // ObjectIndentation :  ��e���� Indentation 
    // Indentation : �ثe����A��W�@�^�X�w�g�ֿn�� Indentation
    // IndentAccum : ���^�X�ֿn(�Ҷq����r��e��)�� Indentation
    // IsGroupHeaderEle : �� ROW �����I�����Ψөw�쪺
    // IsIndentationNecessary : �� ROW ���}�Y�����A�����޵��c���վ𪬪��Y�Ƴ��@�w�n��
    if (!IsGroupHeaderEle and !InspectorConf.IndentationIndependent)    // �O relative �B���O group ���Ĥ@�Ӥ����A�h��۫e���������ڨ̧Ǳ��U�h
        return SetIndentation == -1 ? ImGui::GetCursorPosX() : SetIndentation;

    float UseObjectIndentation = (InspectorConf.IndentationIndependent or IsGroupHeaderEle) ? ObjectIndentation : 0;    // �O�_�ϥ� Indentation      //�p�G�O IsGroupHeaderEle �h�����o�� ObjectIndentation
    float CurrentIndentation = InspectorConf.IndentationAccumulate ? Indentation : IndentAccum;     // Indentation �O�_�Ҷq��C�Ӫ���r�ꥻ�����e��

    if ((InspectorConf.IndentationIndependent or (InspectorConf.AlignGroupObjectIndentation and IsGroupHeaderEle)) and !IsIndentationNecessary)   // �W�ߨϥ� ObjectIndentation// �ϥ� ObjectIndentation �B�Ҽ{�Y��
        return UseObjectIndentation;            // ���S���ƭȨ��M��O�_ UseObjectIndentation
    else
        return UseObjectIndentation + CurrentIndentation;   //�Ҽ{���Y�ơB�Y�ƬO�_�Ҽ{�r����ר��M�� IndentationAccumulate�B �O�Y�ϥ� ObjectIndentation ���M�� UseObjectIndentation 
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
            TempObj.Type = "EnumProperty";  // �ɦV Enum
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
            // ��l��
            EditorMemMgr.MapInitIfNotExist<std::pair<std::string, int>>(EditorMemMgr.ObjectStroageData[BaseAddress].EnumType, UniqueLabel, { "None" , 0 });
            // �]�w
            if (EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].first == "None") {
                for (int i = 0; i < EnumList.size(); ++i) {
                    if (EnumList[i].second == Value) EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel] = { EnumList[i].first, EnumList[i].second };
                }
            }
            // ���
            ImGui::PushItemWidth(100);
            bool IsValueChange = false;
            std::string ShowOption = EditorMemMgr.ObjectStroageData[BaseAddress].EnumType[UniqueLabel].first;
            if (ImGui::BeginCombo(("##Enum_Property" + UniqueLabel).c_str(), ShowOption.substr((ShowOption.find("::") != std::string::npos ? ShowOption.find("::") + 2 : 0)).c_str())) {
                for (int i = 0; i < EnumList.size(); ++i) {
                    const bool IsSelected = (EnumList[i].second == Value);

                    if (ImGui::Selectable((EnumList[i].first.substr((EnumList[i].first.find("::") != std::string::npos ? EnumList[i].first.find("::") + 2 : 0))).c_str(), IsSelected)) {      // + 2 �O���F���L "::"
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
        MemMgr.MemReader.ReadBytes(StrAddress, buffer, 50);     // �r��̱`�HŪ�� 50
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

    // �i�ॼ�ӷ|�䴩
    //ObjectPropertyBase
    //NumericProperty
    //PropertyWrapper
    //DelegateProperty
    //WeakObjectProperty
    //FieldPathProperty
}