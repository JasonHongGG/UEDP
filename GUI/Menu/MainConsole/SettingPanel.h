#pragma once
#include "../../../imgui/imgui.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"

namespace SettingPanel
{
    inline float SlideWidth = 200;
    inline float SectionWidth = 400;
    inline float PanelWindowWidth = 1225;
    inline float PanelWindowHeight = 400;

	void Render()
	{
        if (!MainConsoleState.OpenSettingPanel) return;

		ImGui::SetNextWindowPos(MainConsoleConf.CurrentWindowPosition + ((MainConsoleConf.CurentWindowSize - ImVec2(PanelWindowWidth, PanelWindowHeight)) / 2), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(PanelWindowWidth, PanelWindowHeight), ImGuiCond_FirstUseEver);
        ImGui::Begin("PannelSetting", &MainConsoleState.OpenSettingPanel, ImGuiWindowFlags_HorizontalScrollbar);      //設成 0 就會自動伸縮
        {
            // ============================== PackageViwerPannelSetting ==============================
            ImGui::BeginChild("PackageViwerPannelSetting", ImVec2(SectionWidth, 0), true);
            {
                ImGui::PushFont(Font::TitleText);
                ImGui::Text("PackageViwer Setting");
                ImGui::PopFont();

                ImGui::DummySpace();
                ImGui::DragFloatWithWidth("SlideWidth", SlideWidth, SlideWidth);

                ImGui::BeginGroupPanel("Pannel", ImVec2(-1, 0));
                {
                    ImGui::DummySpace();
                    ImGui::Text("PackageData Resizeable: ");
                    ImGui::RadioButton("Yes##PackageDataResizeable", &PackageViwerConf.PackageDataResizeable, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##PackageDataResizeable", &PackageViwerConf.PackageDataResizeable, false);

                    ImGui::DummySpace();
                    ImGui::Text("ObjectInfo Resizeable: ");
                    ImGui::RadioButton("Yes##ObjectInfo Resizeable", &PackageViwerConf.PackageObjectResizeable, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##ObjectInfo Resizeable", &PackageViwerConf.PackageObjectResizeable, false);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Package Width", PackageViwerConf.PackageDataWindowWidth, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("ObjectInfo Width", PackageViwerConf.PackageObjectWindowWidth, SlideWidth);

                    ImGui::DummySpace();
                }
                ImGui::EndGroupPanel();

                ImGui::BeginGroupPanel("Indentation", ImVec2(-1, 0));
                {
                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("ObjectType", PackageViwerConf.ObjectTypeIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("ObjectName", PackageViwerConf.ObjectNameindentation, SlideWidth);

                    ImGui::DummySpace();
                }
                ImGui::EndGroupPanel();
            }
            ImGui::EndChild();

            // ============================== ObjectInspectorPannelSetting ==============================
            ImGui::SameLine();
            ImGui::BeginChild("ObjectInspectorPannelSetting", ImVec2(SectionWidth, 0), true);
            {
                ImGui::PushFont(Font::TitleText);
                ImGui::Text("ObjectInspector Setting");
                ImGui::PopFont();

                ImGui::DummySpace();
                ImGui::BeginGroupPanel("Pannel", ImVec2(-1, 0));
                {
                    ImGui::DummySpace();
                    ImGui::Text("InspectorList Resizeable: ");
                    ImGui::RadioButton("Yes##InspectorListResizeable", &InspectorConf.InspectorListResizeable, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##InspectorListResizeable", &InspectorConf.InspectorListResizeable, false);

                    ImGui::DummySpace();
                    ImGui::Text("DerivedList Resizeable: ");
                    ImGui::RadioButton("Yes##DerivedListResizeable", &InspectorConf.DerivedListResizeable, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##DerivedListResizeable", &InspectorConf.DerivedListResizeable, false);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("InspectorLsit Width", InspectorConf.InspectorList_Width, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("DerivedList Width", InspectorConf.DerivedList_Width, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Tree Padding", Style::TreeNodePadding, SlideWidth);

                    ImGui::DummySpace();
                }
                ImGui::EndGroupPanel();

                ImGui::DummySpace();
                ImGui::BeginGroupPanel("Indentation", ImVec2(-1, 0));
                {
                    // ======================================
                    // Option
                    ImGui::DummySpace();
                    ImGui::Text("Position");
                    ImGui::RadioButton("Abs", &InspectorConf.IndentationIndependent, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("Rel", &InspectorConf.IndentationIndependent, false);

                    ImGui::DummySpace();
                    ImGui::Text("Indentation Accumulate");
                    ImGui::RadioButton("Yes##IndentationAccumulate", &InspectorConf.IndentationAccumulate, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##IndentationAccumulate", &InspectorConf.IndentationAccumulate, false);

                    ImGui::DummySpace();
                    ImGui::Text("Align GroupObject Indentation");
                    ImGui::RadioButton("Yes##AlignGroupObjectIndentation", &InspectorConf.AlignGroupObjectIndentation, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##AlignGroupObjectIndentation", &InspectorConf.AlignGroupObjectIndentation, false);

                    // ======================================
                    // Space
                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Base", InspectorConf.InspectorBaseIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Offset", InspectorConf.InspectorOffsetIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Type", InspectorConf.InspectorTypeIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Name", InspectorConf.InspectorNameIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Obj Address", InspectorConf.InspectorObjectAddressIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Address", InspectorConf.InspectorAddressIndentation, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Value", InspectorConf.InspectorValueIndentation, SlideWidth);

                    ImGui::DummySpace();
                }
                ImGui::EndGroupPanel();

                ImGui::DummySpace();
                ImGui::BeginGroupPanel("NavLine", ImVec2(-1, 0));
                {
                    ImGui::DummySpace();
                    ImGui::Text("Tree NavLine");
                    ImGui::RadioButton("Yes##TreeNavLine", &InspectorConf.InspectorTreeNavLine, true);
                    ImGui::SameLine();
                    ImGui::RadioButton("No##TreeNavLine", &InspectorConf.InspectorTreeNavLine, false);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("NavLine Offset", InspectorConf.InspectorTreeNavLineOffset, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Object NavLine Offset", InspectorConf.InspectorTreeObjectNavLineOffset, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("Member NavLine Offset", InspectorConf.InspectorTreeMemberNavLineOffset, SlideWidth);

                    ImGui::DummySpace();
                    ImGui::DragFloatWithWidth("NavBranchLine Size", InspectorConf.InspectorTreeNavBranchLineSize, SlideWidth);

                    ImGui::DummySpace();
                }
                ImGui::EndGroupPanel();
            }
            ImGui::EndChild();

            // ============================== ColorPannelSetting ==============================
            ImGui::SameLine();
            ImGui::BeginChild("ColorPannelSetting", ImVec2(SectionWidth, 0), true);
            {
                ImGui::PushFont(Font::TitleText);
                ImGui::Text("Color Setting");
                ImGui::PopFont();

                ImGui::DummySpace();
                ImGui::ColorEdit3("Tab##TabColor", (float*)&Color::TabColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Text##TextColor", (float*)&Color::TextColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Object##ObjectColor", (float*)&Color::ObjectColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("ObjectClickable##ObjectClickableColor", (float*)&Color::ObjectClickableColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Type##TypeColor", (float*)&Color::TypeColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Name##NameColor", (float*)&Color::NameColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("ObjectAddress##ObjectAddressColor", (float*)&Color::ObjectAddressColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Address##AddressColor", (float*)&Color::AddressColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Value##ValueColor", (float*)&Color::ValueColor);

                ImGui::DummySpace();
                ImGui::ColorEdit3("Offest##OffestColor", (float*)&Color::OffestColor);

                ImGui::DummySpace();
                ImGui::ColorEdit4("TreeNodeHover##TreeNodeHoverColor", (float*)&Color::TreeNodeHoverColor);

                ImGui::DummySpace();
                ImGui::ColorEdit4("TreeNavLine##TreeNavLineColor", (float*)&Color::TreeNavLineColor);

                ImGui::DummySpace();
                ImGui::ColorEdit4("Highlight##HighlightColor", (float*)&Color::HighlightColor);
            }
            ImGui::EndChild();
        }
        ImGui::End();
	}
}