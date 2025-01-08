#pragma once
#include "ShaderEffectInterface.h"


class ConnectionLineEffect : public IShaderEffect {
public:
	ConnectionLineEffect(int connectThreshold, int selectColorThreshold)
		: connectThreshold(connectThreshold), selectColorThreshold(selectColorThreshold) {}

    void Render(ImDrawList* d, const EffectParams& params) override
	{
		auto l2 = [](ImVec2 x) {return x.x * x.x + x.y * x.y; };

		int N = 800;
		ImVec2 size = params.sz;
		static auto v = [N, size]() {
			std::vector<std::pair<ImVec2, ImVec2>>r(N);
			for (auto& p : r)
				p.second = p.first = ImVec2(float(rand() % int(size.x)), float(rand() % int(size.y)));
			return r;
			}();

		float D, T;
		for (auto& p : v) {
			D = sqrt(l2(p.first - p.second));	// 兩點之間的距離
			if (D > 0) p.first = p.first + (p.second - p.first) / D;		//p.first 加上單位距離(該距離為兩點之間的距離的單位向量) //第一點逐漸往第二點前進
			if (D < 4) p.second = ImVec2(float(rand() % int(params.sz.x)), float(rand() % int(params.sz.y)));	//隨機指派第二點的位置		//一對 pair 兩點靠太近就隨即把第二點把放到另一個位置
		}

		ImVec2 mousePos = (params.mouse - params.a);	//實際在當前視窗中的位置 (imgui視窗、不是window視窗)
		if (mousePos.x < 0)mousePos.x = 0;
		if (mousePos.y < 0)mousePos.y = 0;
		for (int i = 0; i < N; i++) {
			for (int j = i + 1; j < N; j++) {
				D = l2(v[i].first - v[j].first);		//任兩個 pair 的第一點之間的距離
				T = l2((v[i].first - mousePos + v[j].first - mousePos)) / selectColorThreshold;	//根據兩點的距離相加  減去當前window面積  除400，決定顏色
				//T = D
				if (T > 255) T = 255;
				if (D < connectThreshold) d->AddLine(params.a + v[i].first, params.a + v[j].first, IM_COL32(T, 255 - T, 255, 70), 2);
			}
		}
	}
private:
	int connectThreshold;
	int selectColorThreshold;
};