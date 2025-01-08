#pragma once
#include "ShadowEditor.h"
#include "ToggleEditor.h"
#include "GradientEditor.h"

class ComponentPanelManager
{
public:
	void Render() {
		ShadowEditor::Render();
		ToggleEditor::Render();
		GradientEditor::Render();
	}

private:

};

inline ComponentPanelManager ComponentPanelMgr  = ComponentPanelManager();