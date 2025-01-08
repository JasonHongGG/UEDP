#pragma once
#include "../../../../../imgui/imgui.h"
#include "../../../../Config/ToggleConfig.h"
#include "../../../../Include/Toggle/imgui_toggle.h"
#include "../../../../Include/Toggle/imgui_toggle_presets.h"
//#include "../../../../MyGuiComponent.h"

namespace ToggleEditor
{
	void imgui_toggle_state(const ImGuiToggleConfig& config, ImGuiToggleStateConfig& state);

	void imgui_toggle_custom();

	void Render()
	{
		bool& state = ConfigPageComponentStateMgr.OpenToggleEditor;

		ImGui::SetNextWindowSize({ 620,700 }, ImGuiCond_FirstUseEver);
		if (state and ImGui::Begin("ToggleEditor", &state)) {
			imgui_toggle_custom();
			ImGui::End();
		}
	}
}

void ToggleEditor::imgui_toggle_state(const ImGuiToggleConfig& config, ImGuiToggleStateConfig& state)
{
	// some values to use for slider limits
	const float border_thickness_max_pixels = 50.0f;
	const float max_height = config.Size.y > 0 ? config.Size.y : ImGui::GetFrameHeight();
	const float half_max_height = max_height * 0.5f;

	// knob offset controls how far into or out of the frame the knob should draw.
	ImGui::SliderFloat2("Knob Offset (px: x, y)", &state.KnobOffset.x, -half_max_height, half_max_height);

	// knob inset controls how many pixels the knob is set into the frame. negative values will cause it to grow outside the frame.
	// for circular knobs, we will just use a single value, while for we will use top/left/bottom/right offsets.
	const bool is_rounded = config.KnobRounding >= 1.0f;
	if (is_rounded)
	{
		float inset_average = state.KnobInset.GetAverage();
		ImGui::SliderFloat("Knob Inset (px)", &inset_average, -half_max_height, half_max_height);
		state.KnobInset = inset_average;
	}
	else
	{
		ImGui::SliderFloat4("Knob Inset (px: t, l, b, r)", state.KnobInset.Offsets, -half_max_height, half_max_height);
	}

	// how thick should the frame border be (if enabled)
	ImGui::SliderFloat("Frame Border Thickness (px)", &state.FrameBorderThickness, 0.0f, border_thickness_max_pixels);

	// how thick should the knob border be (if enabled)
	ImGui::SliderFloat("Knob Border Thickness (px)", &state.KnobBorderThickness, 0.0f, border_thickness_max_pixels);
}

void ToggleEditor::imgui_toggle_custom()
{
	static bool toggle_custom = true;

	ImGui::NewLine();
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ToggleConf.Size.x) / 2);
	ImGui::Toggle("##Customized Toggle", &toggle_custom, ToggleConf);
	ImGui::NewLine();

	if (ImGui::Button("Reset to Default"))
		ToggleConf = ImGuiTogglePresets::DefaultStyle();

	// these first settings are used no matter the toggle's state.
	ImGui::Text("Persistent Toggle Settings");

	// animation duration controls how long the toggle animates, in seconds. if set to 0, animation is disabled.
	if (ImGui::SliderFloat("Animation Duration (seconds)", &ToggleConf.AnimationDuration, ImGuiToggleConstants::AnimationDurationMinimum, 2.0f))
	{
		// if the user adjusted the animation duration slider, go ahead and turn on the animation flags.
		ToggleConf.Flags |= ImGuiToggleFlags_Animated;
	}

	// frame rounding sets how round the frame is when drawn, where 0 is a rectangle, and 1 is a circle.
	ImGui::SliderFloat("Frame Rounding (scale)", &ToggleConf.FrameRounding, ImGuiToggleConstants::FrameRoundingMinimum, ImGuiToggleConstants::FrameRoundingMaximum);

	// knob rounding sets how round the knob is when drawn, where 0 is a rectangle, and 1 is a circle.
	ImGui::SliderFloat("Knob Rounding (scale)", &ToggleConf.KnobRounding, ImGuiToggleConstants::KnobRoundingMinimum, ImGuiToggleConstants::KnobRoundingMaximum);

	// size controls the width and the height of the toggle frame
	ImGui::SliderFloat2("Size (px: w, h)", &ToggleConf.Size.x, 0.0f, 200.0f, "%.0f");

	// width ratio sets how wide the toggle is with relation to the frame height. if Size is non-zero, this is unused.
	ImGui::SliderFloat("Width Ratio (scale)", &ToggleConf.WidthRatio, ImGuiToggleConstants::WidthRatioMinimum, ImGuiToggleConstants::WidthRatioMaximum);

	// a11y style sets the type of additional on/off indicator drawing
	if (ImGui::Combo("A11y Style", &ToggleConf.A11yStyle,
		"Label\0"
		"Glyph\0"
		"Dot\0"
		"\0"))
	{
		// if the user adjusted the a11y style combo, go ahead and turn on the a11y flag.
		ToggleConf.Flags |= ImGuiToggleFlags_A11y;
	}

	// some tabs to adjust the "state" settings of the toggle (configuration dependent on if the toggle is on or off.)
	if (ImGui::BeginTabBar("State"))
	{
		if (ImGui::BeginTabItem("\"Off State\" Settings"))
		{
			imgui_toggle_state(ToggleConf, ToggleConf.Off);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("\"On State\"Settings"))
		{
			imgui_toggle_state(ToggleConf, ToggleConf.On);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::Separator();

	// flags for various toggle features
	ImGui::Text("Flags");
	ImGui::Columns(2);
	ImGui::Text("Meta Flags");
	ImGui::NextColumn();
	ImGui::Text("Individual Flags");
	ImGui::Separator();
	ImGui::NextColumn();
	{
		// should the toggle have borders (sets all border flags)
		ImGui::CheckboxFlags("Bordered", &ToggleConf.Flags, ImGuiToggleFlags_Bordered);

		// should the toggle have shadows (sets all shadow flags)
		ImGui::CheckboxFlags("Shadowed", &ToggleConf.Flags, ImGuiToggleFlags_Shadowed);
	}

	ImGui::NextColumn();
	{
		// should the toggle animate
		ImGui::CheckboxFlags("Animated", &ToggleConf.Flags, ImGuiToggleFlags_Animated);

		// should the toggle have a bordered frame
		ImGui::CheckboxFlags("BorderedFrame", &ToggleConf.Flags, ImGuiToggleFlags_BorderedFrame);

		// should the toggle have a bordered knob
		ImGui::CheckboxFlags("BorderedKnob", &ToggleConf.Flags, ImGuiToggleFlags_BorderedKnob);

		// should the toggle have a shadowed frame
		ImGui::CheckboxFlags("ShadowedFrame", &ToggleConf.Flags, ImGuiToggleFlags_ShadowedFrame);

		// should the toggle have a shadowed knob
		ImGui::CheckboxFlags("ShadowedKnob", &ToggleConf.Flags, ImGuiToggleFlags_ShadowedKnob);

		// should the toggle draw a11y glyphs
		ImGui::CheckboxFlags("A11y", &ToggleConf.Flags, ImGuiToggleFlags_A11y);
	}

}


