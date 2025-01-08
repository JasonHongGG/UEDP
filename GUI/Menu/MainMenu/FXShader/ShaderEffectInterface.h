#pragma once
#include <vector>
#include "../../../../imgui/imgui.h"
#include "../../../MyGuiComponent/Operator.h"

struct EffectParams {
    ImVec2 a;
    ImVec2 b;
    ImVec2 sz;
    ImVec2 mouse;
    float t;
};

class IShaderEffect {
public:
    virtual void Render(ImDrawList* d, const EffectParams& params) = 0;
    virtual ~IShaderEffect() = default;
};
