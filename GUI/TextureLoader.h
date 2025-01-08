#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <algorithm>
#include "../Utils/Include/StbImage.h"
#include "../Utils/Include/nanosvg.h"
#include "../Utils/Include/nanosvgrast.h"

class TextureLoader
{
public:
	int* ImageDelays = NULL;
	int ImageWidth = 0;
	int ImageHeight = 0;
	int FrameCnt = 0; // mayby will sport gif
	int FrameDisplayCnt = 0;
	int FrameTimer = 3;	// ¨C 3s ¸õ¤U¤@­s Frame
	float ImageProportion = 0;
	std::vector<ID3D11ShaderResourceView*> ImageTextureVec;

	stbi_uc* LoadSVG(const char* filename, int* width, int* height, int* channels);

	stbi_uc* stbi_load_gif(char const* filename, int** delays, int* width, int* height, int* frames, int* nrChannels, int req_comp);

	stbi_uc* stbi_xload(char const* filename, int* width, int* height, int** delays, int* frames, int* nrChannels);

	bool LoadTextureFromFile(ID3D11Device* Device, const char* filename, std::vector<ID3D11ShaderResourceView*>& out_srv, int* out_width, int* out_height, int* out_frame);

private:

};

inline TextureLoader ImageLoader = TextureLoader();