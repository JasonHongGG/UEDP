#pragma once
#include "../../imgui/imgui.h"
#include "../../Background/Mono/MonoManager.h"

namespace MonoPanel
{
	void Render(HWND window)
	{
		if (!MonoMgr.Enable) return;

		ImGui::Begin("Mono");
		{
			MonoMgr.TestLoop(window);
			if (ImGui::Button("Heal")) {
				MonoMgr.HealFeature();
			}
			if (ImGui::Button("TackDamage")) {
				MonoMgr.TakeDamageFeature();
			}
			if (ImGui::Button("Collision")) {
				MonoMgr.CollisionFeature();
			}
			if (ImGui::Button("Gravity")) {
				MonoMgr.GravityFeature();
			}
		}
		ImGui::End();
	}
}