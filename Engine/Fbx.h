#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include"Direct3D.h"
#include"Camera.h"
#include<vector>

class Texture;

#pragma comment(lib, "LibFbxSDK-Md.lib")
#pragma comment(lib, "LibXml2-Md.lib")
#pragma comment(lib, "zlib-Md.lib")



class Fbx
{
	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4	diffuse;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMFLOAT4		diffuseColor;		// ディフューズカラー（マテリアルの色）
		int		isTexture;
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};
	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数 だいたいvertex*3になるでしょう
	int materialCount_;	//マテリアルの個数
	int *indexCount_;

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