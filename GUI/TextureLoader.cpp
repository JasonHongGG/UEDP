#pragma once
#define STB_IMAGE_IMPLEMENTATION	//只能定義一次
//svg
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "TextureLoader.h"

stbi_uc* TextureLoader::LoadSVG(const char* filename, int* width, int* height, int* channels)
{
	// Load SVG image
	struct NSVGimage* image = nsvgParseFromFile(filename, "px", 96.0f);
	if (!image) {
		return nullptr;
	}

	*width = (int)image->width;
	*height = (int)image->height;
	*channels = 4; // RGBA

	// Allocate memory for the output bitmap
	unsigned char* bitmap = (unsigned char*)malloc(*width * *height * *channels);
	if (!bitmap) {
		nsvgDelete(image);
		return nullptr;
	}

	// Create rasterizer and rasterize the SVG image
	NSVGrasterizer* rast = nsvgCreateRasterizer();
	nsvgRasterize(rast, image, 0, 0, 1, bitmap, *width, *height, *width * *channels);

	// Clean up
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);

	return bitmap;
}

stbi_uc* TextureLoader::stbi_load_gif(char const* filename, int** delays, int* width, int* height, int* frames, int* nrChannels, int req_comp)
{
	unsigned char* result;
	FILE* f = stbi__fopen(filename, "rb");
	if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
	fseek(f, 0, SEEK_END);
	int bufSize = ftell(f);
	unsigned char* buf = (unsigned char*)malloc(sizeof(unsigned char) * bufSize);
	if (buf)
	{
		fseek(f, 0, SEEK_SET);
		fread(buf, 1, bufSize, f);
		result = stbi_load_gif_from_memory(buf, bufSize, delays, width, height, frames, nrChannels, req_comp);
		free(buf);
		buf = NULL;
	}
	else
	{
		result = stbi__errpuc("outofmem", "Out of memory");
	}
	fclose(f);
	return result;
}

stbi_uc* TextureLoader::stbi_xload(char const* filename, int* width, int* height, int** delays, int* frames, int* nrChannels)
{
	FILE* f = stbi__fopen(filename, "rb");
	if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
	fclose(f);

	std::string file_ext = std::string(filename).substr(std::string(filename).find_last_of(".") + 1);
	std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

	if (file_ext == "gif") {
		return stbi_load_gif(filename, delays, width, height, frames, nrChannels, 4);
	}
	else if (file_ext == "svg") {
		*frames = 1; // No animation support for SVG
		return LoadSVG(filename, width, height, nrChannels);
	}
	else {
		*frames = 1;
		return stbi_load(filename, width, height, nrChannels, 4);
	}
}

bool TextureLoader::LoadTextureFromFile(ID3D11Device* Device, const char* filename, std::vector<ID3D11ShaderResourceView*>& out_srv, int* out_width, int* out_height, int* out_frame)
{
	// Load from disk into a raw RGBA buffer
	int width = 0, height = 0, frames = 1, channels = 0;
	if (ImageDelays == NULL) ImageDelays = (int*)malloc(sizeof(int) * 0x50);
	if (ImageDelays) ImageDelays[0] = 0;

	//unsigned char* image_data = stbi_load(filename, &width, &height, NULL, 4);
	unsigned char* image_data = stbi_xload(filename, &width, &height, &ImageDelays, &frames, &channels);
	if (image_data == NULL)
		return false;

	size_t frame_size = width * height * channels;
	for (int i = 0; i < frames; i++)
	{
		ID3D11ShaderResourceView* TempSRV = NULL;
		out_srv.push_back(TempSRV);

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = image_data + i * frame_size;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		Device->CreateTexture2D(&desc, &subResource, &pTexture);
		if (!pTexture) return false;

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Device->CreateShaderResourceView(pTexture, &srvDesc, &out_srv[i]);
		pTexture->Release();

	}

	*out_width = width;
	*out_height = height;
	*out_frame = frames;
	stbi_image_free(image_data);
	return true;
}


