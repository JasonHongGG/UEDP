#pragma once
#include "ShadowEditor.h"
#include "ToggleEditor.h"

class ComponentPanelManager
{
public:
	void Render() {
		ShadowEditor::Render();
		ToggleEditor::Render();
	}

private:

};

inline ComponentPanelManager ComponentPanelMgr  = ComponentPanelManager();