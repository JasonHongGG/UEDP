#pragma once
#include "ShaderEffectInterface.h"

#ifndef min
#define min(x,y) ((x)<(y)?x:y)
#endif

#define wh(a) ImColor(1.f,1.f,1.f,a)

class RainEffect : public IShaderEffect {
public:
    void Render(ImDrawList* d, const EffectParams& params) override {
		static float fl;
		if ((rand() % 500) == 0) fl = params.t;
		if ((params.t - fl) > 0)
		{
			auto ft = 0.25f;
			d->AddRectFilled(params.a, params.b, wh((ft - (params.t - fl)) / ft));
		}

		for (int i = 0; i < 2000; ++i) {
			unsigned h = ImGui::GetID(d + i + int(params.t / 4));
			auto f = fmodf(params.t + fmodf(h / 777.f, 99), 99);
			auto tx = h % (int)params.sz.x;
			auto ty = h % (int)params.sz.y;
			if (f < 1) {
				auto py = ty - 1000 * (1 - f);
				d->AddLine({ params.a.x + tx, params.a.y + py }, { params.a.x + tx, params.a.y + min(py + 10,ty) }, (ImU32)-1);
			}
			else if (f < 1.2f)
				d->AddCircle({ params.a.x + tx, params.a.y + ty }, (f - 1) * 10 + h % 5, wh(1 - (f - 1) * 5.f));
		}
    }
};