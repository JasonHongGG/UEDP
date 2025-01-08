#pragma once
#include <d3d11.h>
#include "ShaderEffectInterface.h"

struct IconFollowerParams : public EffectParams {
	
};

class IconFollowerEffect : public IShaderEffect
{
public:
	IconFollowerEffect(ID3D11ShaderResourceView* ImgTexture,	ImVec2 ImgSize,	float FollowSpeed)
		: ImgTexture(ImgTexture), ImgSize(ImgSize), FollowSpeed(FollowSpeed){}

	void Render(ImDrawList* d, const EffectParams& params)
	{
		auto l2 = [](ImVec2 x) {return x.x * x.x + x.y * x.y; };

		ImVec2 mousePos = (params.mouse - params.a);
		if (params.mouse >= params.a and params.mouse <= params.b) {
			static ImVec2 ImagePos = params.a + (params.sz / 2) - (ImgSize / 2);	//正中心
			float D = sqrt(l2(params.mouse - (ImagePos + (ImgSize / 2))));	// 兩點之間的距離
			if ((int)D > 20) ImagePos = ImagePos + (params.mouse - ImagePos) / D * (FollowSpeed == 0 ? 1 : FollowSpeed);
			d->AddImage((void*)ImgTexture, ImagePos, ImagePos + ImgSize);
		}
	}
private:
	ID3D11ShaderResourceView* ImgTexture;
	ImVec2 ImgSize;
	float FollowSpeed;
};