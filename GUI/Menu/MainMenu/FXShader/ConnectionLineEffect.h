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
			D = sqrt(l2(p.first - p.second));	// ���I�������Z��
			if (D > 0) p.first = p.first + (p.second - p.first) / D;		//p.first �[�W���Z��(�ӶZ�������I�������Z�������V�q) //�Ĥ@�I�v�����ĤG�I�e�i
			if (D < 4) p.second = ImVec2(float(rand() % int(params.sz.x)), float(rand() % int(params.sz.y)));	//�H�������ĤG�I����m		//�@�� pair ���I�a�Ӫ�N�H�Y��ĤG�I����t�@�Ӧ�m
		}

		ImVec2 mousePos = (params.mouse - params.a);	//��ڦb��e����������m (imgui�����B���Owindow����)
		if (mousePos.x < 0)mousePos.x = 0;
		if (mousePos.y < 0)mousePos.y = 0;
		for (int i = 0; i < N; i++) {
			for (int j = i + 1; j < N; j++) {
				D = l2(v[i].first - v[j].first);		//����� pair ���Ĥ@�I�������Z��
				T = l2((v[i].first - mousePos + v[j].first - mousePos)) / selectColorThreshold;	//�ھڨ��I���Z���ۥ[  ��h��ewindow���n  ��400�A�M�w�C��
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