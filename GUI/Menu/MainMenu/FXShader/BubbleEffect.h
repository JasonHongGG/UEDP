#pragma once
#include "ShaderEffectInterface.h"

class BubbleEffect : public IShaderEffect
{
public:
	void Render(ImDrawList* d, const EffectParams& baseParams) override
	{
		static int i{};
		static P p[64];	//陣列的數量就式 Bubble 數量
		float k;
		int j{};
		if (!i) { i = 1; for (P& a : p) { a = { a.r(),a.r(),a.r() + 5.f,a.r(),a.r(),a.r() }; } }
		for (P& o : p) {
			if (o.z < 0.001) continue;
			ImVec2 s((o.x / o.z) * baseParams.sz.x * 2.f + baseParams.sz.x * 0.5f + baseParams.a.x, (o.y / o.z) * baseParams.sz.y * 2.f + baseParams.sz.y * 0.5f + baseParams.a.y);
			int x = (j++) % 16;
			k = cosf((j / 64.f) * 3.14592f) * 0.002f + 0.002f;
			float dist = fabsf(o.z - 5.f) / 2.5f, sc = (10.f + dist * 100.f) / o.z;
			int tr = int(ImMin(dist * 128.f, 128.f) + 127) << 24;
			ImColor col = ImColor::HSV(k * 9.f + 0.06f, 0.8f, 1.f, 1.f - sqrtf(dist));
			d->AddCircleFilled(s, sc, col, 12); o.A(k);
		}
	}
private:
	struct P {
		float x, y, z, a, b, c; void A(float k) {
			x += a * k; y += b * k; z += c * k;
			float ng{ 0.008f }; z -= 5.f; float xp = x * cosf(ng) - z * sinf(ng); float zp = x * sinf(ng) + z * cosf(ng);
			x = xp; z = zp + 5.f; a -= x * k + r() * k; b -= y * k + r() * k; c -= (z - 5.0f) * k + r() * k;
		}
		float r() { return float(rand() / 32768.f) * 2.f - 1.f; };
	};
};