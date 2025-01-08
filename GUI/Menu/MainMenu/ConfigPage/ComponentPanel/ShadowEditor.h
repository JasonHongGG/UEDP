#pragma once
#include "../../../../../imgui/imgui.h"
#include "../../../../MyGuiComponent.h"

#include "../../../../Config/ShadowConfig.h"
#include "../../../../../State/GUIState.h"

namespace ShadowEditor
{
	void Render()
	{
		bool& state = ConfigPageComponentStateMgr.OpenShadowEditor;

		if (state and ImGui::Begin("StyleEditor", &state))
		{
			ImGui::Checkbox("Shadow filled", &ShadowConf.shadow_filled);
			ImGui::SameLine();
			ImGui::HelpMarker("This will fill the section behind the shape to shadow. It's often unnecessary and wasteful but provided for consistency.");
			ImGui::Checkbox("Wireframe shapes", &ShadowConf.wireframe);
			ImGui::SameLine();
			ImGui::HelpMarker("This draws the shapes in wireframe so you can see the shadow underneath.");
			ImGui::Checkbox("Anti-aliasing", &ShadowConf.aa);

			ImGui::DragFloat("Shadow Thickness", &ShadowConf.shadow_thickness, 1.0f, 0.0f, 100.0f, "%.02f");
			ImGui::SliderFloat2("Offset", (float*)&ShadowConf.shadow_offset, -32.0f, 32.0f);
			ImGui::SameLine();
			ImGui::HelpMarker("Note that currently circles/convex shapes do not support non-zero offsets for unfilled shadows.");
			ImGui::ColorEdit4("Background Color", &ShadowConf.background_color.x);
			ImGui::ColorEdit4("Shadow Color", &ShadowConf.shadow_color.x);
			ImGui::ColorEdit4("Shape Color", &ShadowConf.shape_color.x);
			ImGui::DragFloat("Shape Rounding", &ShadowConf.shape_rounding, 1.0f, 0.0f, 20.0f, "%.02f");
			ImGui::Combo("Convex shape", &ShadowConf.poly_shape_index, "Shape 1\0Shape 2\0Shape 3\0Shape 4\0Shape 4 (winding reversed)");

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImDrawListFlags old_flags = draw_list->Flags;

			if (ShadowConf.aa)
				draw_list->Flags |= ~ImDrawListFlags_AntiAliasedFill;
			else
				draw_list->Flags &= ~ImDrawListFlags_AntiAliasedFill;

			// Fill a strip of background
			{
				ImVec2 p = ImGui::GetCursorScreenPos();
				draw_list->AddRectFilled(p, ImVec2(p.x + ImGui::GetContentRegionAvail().x, p.y + 200.0f), ImGui::GetColorU32(ShadowConf.background_color));
			}

			// Rectangle
			{
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(200.0f, 200.0f));

				ImVec2 r1(p.x + 50.0f, p.y + 50.0f);
				ImVec2 r2(p.x + 150.0f, p.y + 150.0f);
				ImDrawFlags draw_flags = ShadowConf.shadow_filled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground;
				draw_list->AddShadowRect(r1, r2, ImGui::GetColorU32(ShadowConf.shadow_color), ShadowConf.shadow_thickness, ShadowConf.shadow_offset, draw_flags, ShadowConf.shape_rounding);

				if (ShadowConf.wireframe)
					draw_list->AddRect(r1, r2, ImGui::GetColorU32(ShadowConf.shape_color), ShadowConf.shape_rounding);
				else
					draw_list->AddRectFilled(r1, r2, ImGui::GetColorU32(ShadowConf.shape_color), ShadowConf.shape_rounding);
			}

			ImGui::SameLine();

			// Circle
			{
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(200.0f, 200.0f));

				// FIXME-SHADOWS: Offset forced to zero when shadow is not filled because it isn't supported
				float off = 10.0f;
				ImVec2 r1(p.x + 50.0f + off, p.y + 50.0f + off);
				ImVec2 r2(p.x + 150.0f - off, p.y + 150.0f - off);
				ImVec2 center(p.x + 100.0f, p.y + 100.0f);
				ImDrawFlags draw_flags = ShadowConf.shadow_filled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground;
				draw_list->AddShadowCircle(center, 50.0f, ImGui::GetColorU32(ShadowConf.shadow_color), ShadowConf.shadow_thickness, ShadowConf.shadow_filled ? ShadowConf.shadow_offset : ImVec2(0.0f, 0.0f), draw_flags, 0);

				if (ShadowConf.wireframe)
					draw_list->AddCircle(center, 50.0f, ImGui::GetColorU32(ShadowConf.shape_color), 0);
				else
					draw_list->AddCircleFilled(center, 50.0f, ImGui::GetColorU32(ShadowConf.shape_color), 0);
			}

			ImGui::SameLine();

			// Convex shape
			{
				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(200.0f, 200.0f));

				const ImVec2 poly_centre(pos.x + 50.0f, pos.y + 100.0f);
				ImVec2 poly_points[8];
				int poly_points_count = 0;

				switch (ShadowConf.poly_shape_index)
				{
				default:
				case 0:
				{
					poly_points[0] = ImVec2(poly_centre.x - 32.0f, poly_centre.y);
					poly_points[1] = ImVec2(poly_centre.x - 24.0f, poly_centre.y + 24.0f);
					poly_points[2] = ImVec2(poly_centre.x, poly_centre.y + 32.0f);
					poly_points[3] = ImVec2(poly_centre.x + 24.0f, poly_centre.y + 24.0f);
					poly_points[4] = ImVec2(poly_centre.x + 32.0f, poly_centre.y);
					poly_points[5] = ImVec2(poly_centre.x + 24.0f, poly_centre.y - 24.0f);
					poly_points[6] = ImVec2(poly_centre.x, poly_centre.y - 32.0f);
					poly_points[7] = ImVec2(poly_centre.x - 32.0f, poly_centre.y - 32.0f);
					poly_points_count = 8;
					break;
				}
				case 1:
				{
					poly_points[0] = ImVec2(poly_centre.x + 40.0f, poly_centre.y - 20.0f);
					poly_points[1] = ImVec2(poly_centre.x, poly_centre.y + 32.0f);
					poly_points[2] = ImVec2(poly_centre.x - 24.0f, poly_centre.y - 32.0f);
					poly_points_count = 3;
					break;
				}
				case 2:
				{
					poly_points[0] = ImVec2(poly_centre.x - 32.0f, poly_centre.y);
					poly_points[1] = ImVec2(poly_centre.x, poly_centre.y + 32.0f);
					poly_points[2] = ImVec2(poly_centre.x + 32.0f, poly_centre.y);
					poly_points[3] = ImVec2(poly_centre.x, poly_centre.y - 32.0f);
					poly_points_count = 4;
					break;
				}
				case 3:
				{
					poly_points[0] = ImVec2(poly_centre.x - 4.0f, poly_centre.y - 20.0f);
					poly_points[1] = ImVec2(poly_centre.x + 12.0f, poly_centre.y + 2.0f);
					poly_points[2] = ImVec2(poly_centre.x + 8.0f, poly_centre.y + 16.0f);
					poly_points[3] = ImVec2(poly_centre.x, poly_centre.y + 32.0f);
					poly_points[4] = ImVec2(poly_centre.x - 16.0f, poly_centre.y - 32.0f);
					poly_points_count = 5;
					break;
				}
				case 4: // Same as test case 3 but with reversed winding
				{
					poly_points[0] = ImVec2(poly_centre.x - 16.0f, poly_centre.y - 32.0f);
					poly_points[1] = ImVec2(poly_centre.x, poly_centre.y + 32.0f);
					poly_points[2] = ImVec2(poly_centre.x + 8.0f, poly_centre.y + 16.0f);
					poly_points[3] = ImVec2(poly_centre.x + 12.0f, poly_centre.y + 2.0f);
					poly_points[4] = ImVec2(poly_centre.x - 4.0f, poly_centre.y - 20.0f);
					poly_points_count = 5;
					break;
				}
				}

				// FIXME-SHADOWS: Offset forced to zero when shadow is not filled because it isn't supported
				ImDrawFlags draw_flags = ShadowConf.shadow_filled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground;
				draw_list->AddShadowConvexPoly(poly_points, poly_points_count, ImGui::GetColorU32(ShadowConf.shadow_color), ShadowConf.shadow_thickness, ShadowConf.shadow_filled ? ShadowConf.shadow_offset : ImVec2(0.0f, 0.0f), draw_flags);

				if (ShadowConf.wireframe)
					draw_list->AddPolyline(poly_points, poly_points_count, ImGui::GetColorU32(ShadowConf.shape_color), true, 1.0f);
				else
					draw_list->AddConvexPolyFilled(poly_points, poly_points_count, ImGui::GetColorU32(ShadowConf.shape_color));
			}

			draw_list->Flags = old_flags;
			ImGui::End();
		}
	}
}