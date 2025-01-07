#pragma once
#include <vector>
#include <d3d11.h>
#include "../../Utils/Env.h"

struct LogConfig
{
	std::string LogoPathStr = Const::LogoPathStr;	//logo.svg
	int LogoWidth = 0;
	int LogoHeight = 0;
	int LogoFrame = 0; // mayby will sport gif
	float LogoProportion = 0;
	std::vector<ID3D11ShaderResourceView*> LogoTextureVec;
};

inline LogConfig LogConf;