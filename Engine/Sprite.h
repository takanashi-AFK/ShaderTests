#pragma once

#include <DirectXMath.h>
#include "Direct3D.h"
#include"Texture.h"
#include<vector>
#include<d3d11.h>
#include"Transform.h"


using namespace DirectX;


class Sprite
{
//�R���X�^���g�o�b�t�@
struct CONSTANT_BUFFER
{
	XMMATRIX	matW;
};

//���_���
struct VERTEX
{
	XMVECTOR position;
	XMVECTOR uv;
};

protected:
	
	std::vector<VERTEX> vertices_;
	ID3D11Buffer* pVertexBuffer_;	//���_�o�b�t�@
	TexMetadata image_;
	
	std::vector<int>index_;
	ID3D11Buffer* pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;	//�R���X�^���g�o�b�t�@
	Texture* pTexture_;
	int vertexNum_;
	int indexNum_;
public:
	Sprite();
	~Sprite();
	HRESULT Initialize();
	void Draw(Transform& transform);
	void Release();
private:

	virtual void InitVertexData(int winH,int winW);
	HRESULT CreateVertexBuffer();

	virtual void InitIndexData();
	HRESULT CreateIndexBuffer();

	HRESULT CreateConstantBuffer();

	HRESULT LoadTexture();

	void PassDataToCB(XMMATRIX worldMatrix);
	
	void SetBufferToPipeline();
};