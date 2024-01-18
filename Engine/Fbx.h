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
	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		Texture* pNormalTexture;
		XMFLOAT4	ambient;
		XMFLOAT4	diffuse;
		XMFLOAT4 specular;
		float	shininess;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matW;
		XMMATRIX	matWVP;//ワールドビュープロジェクション
		XMMATRIX	matNormal;//スケール*平行移動の逆行列
		XMFLOAT4	diffuseColor;
		XMFLOAT4	ambientColor;
		XMFLOAT4	specularColor;
		BOOL			isTexture;//これなんでintなん？あとでboolに直して試してみる
		BOOL			isNormalMap;

	};

	struct VERTEX
	{
		XMVECTOR position;//位置
		XMVECTOR uv;//テクスチャ座標
		XMVECTOR normal;//法線
		XMVECTOR tangent;//接線
	};

	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数 だいたいvertex*3になるでしょう
	int materialCount_;	//マテリアルの個数
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