#include <wincodec.h>
#include"Direct3D.h"
#include "Texture.h"

using namespace DirectX;
Texture::Texture():pSampler_(nullptr),pSRV_(nullptr)
{
}

Texture::~Texture()
{
	Release();
}

HRESULT Texture::Load(string filename)
{

	////////////�摜�ǂݍ���////////////
	wchar_t wtext[FILENAME_MAX];

	size_t ret;
	mbstowcs_s(&ret, wtext, filename.c_str(), filename.length());

	ScratchImage image;
	HRESULT hr;
	hr = LoadFromWICFile(wtext, WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}
	////////////////////////////////////
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pSampler_);
	 
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr =CreateShaderResourceView(Direct3D::pDevice_,image.GetImages(),image.GetImageCount(),metadata ,&pSRV_);
	if (FAILED(hr)) {
		return E_FAIL;
	}

	return S_OK;
	
}

void Texture::Release()
{
	SAFE_RELEASE(pSampler_);
	SAFE_RELEASE(pSRV_);
}
