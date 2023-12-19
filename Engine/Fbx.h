#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include"Direct3D.h"
#include"Camera.h"
#include<vector>
#include <DirectXMath.h>

class Texture;

#pragma comment(lib, "LibFbxSDK-Md.lib")
#pragma comment(lib, "LibXml2-Md.lib")
#pragma comment(lib, "zlib-Md.lib")

class Fbx
{
	//�}�e���A��
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4	ambient;
		XMFLOAT4	diffuse;
		XMFLOAT4 specular;
		float	shininess;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matW;
		XMMATRIX	matWVP;//���[���h�r���[�v���W�F�N�V����
		XMMATRIX	matNormal;//�X�P�[��*���s�ړ��̋t�s��
		XMFLOAT4	diffuseColor;// �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
		int			isTexture;
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};

	int vertexCount_;	//���_��
	int polygonCount_;	//�|���S���� ��������vertex*3�ɂȂ�ł��傤
	int materialCount_;	//�}�e���A���̌�
	int *indexCount_;

	Texture *pToonTex;

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	MATERIAL* pMaterialList_;

public:

	Fbx();
	HRESULT Load(std::string fileName);
	void    Draw(Transform& transform);
	void    Release();
	void	InitVertex(fbxsdk::FbxMesh* mesh);
	void	InitIndex(fbxsdk::FbxMesh* mesh);
	void	InitMaterial(fbxsdk::FbxNode* pNode);
	void	IntConstantBuffer();

	void	SetBufferToPipeline(Transform transform);
};