#pragma once
#include <d3d11.h>
#include <string>
#include <wrl.h>

#include <DirectXTex.h>
#pragma comment (lib, "DirectXTex.lib")

using namespace Microsoft::WRL;
using std::string;

class Texture
{
	ID3D11SamplerState* pSampler_;
	ID3D11ShaderResourceView* pSRV_;
	DirectX::TexMetadata metadata;

public:
	Texture();
	~Texture();
	HRESULT Load(string fileName);
	void Release();

	ID3D11SamplerState* GetSampler() { return pSampler_; }
	ID3D11ShaderResourceView* GetSRV() { return pSRV_; }
	DirectX::TexMetadata GetMetaData() { return metadata; }
}; 