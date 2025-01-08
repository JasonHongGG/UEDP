#pragma once
#include "ShaderEffectInterface.h"
#include <memory>

class FXShaderManager
{
private:
    std::vector<std::unique_ptr<IShaderEffect>> effects;
public:
    void AddEffect(std::unique_ptr<IShaderEffect> effect) {
        effects.push_back(std::move(effect));
    }

    void ClearEffect() {
        effects.clear();
    }

    void RenderAll(ImDrawList* d, const EffectParams& params) {
        for (auto& effect : effects) {
            effect->Render(d, params);
        }
    }
};

inline FXShaderManager FXShaderMgr = FXShaderManager();