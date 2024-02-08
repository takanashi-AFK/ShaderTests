#include "Fbx.h"
#include"Texture.h"
#include"../LightPosController.h"
using namespace std;
Fbx::Fbx():pVertexBuffer_(nullptr),pIndexBuffer_(nullptr), pConstantBuffer_(nullptr),vertexCount_(0),polygonCount_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{
	//マネージャを生成
	FbxManager* pFbxManager = FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//シーンオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	//メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode();
	FbxNode* pNode = rootNode->GetChild(0);
	FbxMesh* mesh = pNode->GetMesh();

	//各情報の個数を取得
	vertexCount_ = mesh->GetControlPointsCount();	//頂点の数
	polygonCount_ = mesh->GetPolygonCount();	//ポリゴンの数
	materialCount_ = pNode->GetMaterialCount();


	//現在のカレントディレクトリを取得
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//引数のfileNameからディレクトリ部分を取得
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
	//カレントディレクトリを変更
	SetCurrentDirectory(dir);


	InitVertex(mesh);	//頂点バッファ準備
	InitIndex(mesh);	//インデックスバッファ準備
	IntConstantBuffer();	//コンスタントバッファ準備
	InitMaterial(pNode);

	//終わったら戻す
	SetCurrentDirectory(defaultCurrentDir);


	//マネージャ解放
	pFbxManager->Destroy();

	pToonTex = new Texture;

	pToonTex->Load("Assets/toonTexture.png");
	return S_OK;
}

void Fbx::Draw(Transform& transform)
{
	scrollValX_ += 0.001f;
	scrollValY_ += 0.00079f;
	Direct3D::SetShader(SHADER_NORMAL);
	transform.Calclation();

	//頂点バッファ、インデックスバッファ、コンスタントバッファをパイプラインにセット
	SetBufferToPipeline(transform);

}


void Fbx::InitVertex(fbxsdk::FbxMesh* mesh)
{
	//頂点情報を入れる配列
	VERTEX* vertices = new VERTEX[vertexCount_];

	//全ポリゴン
	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//調べる頂点の番号
			//1つのポリゴンを構成する頂点のインデックス番号のセットを得る関数
			int index = mesh->GetPolygonVertex(poly, vertex);

			//頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//頂点のUV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);
			//頂点の法線
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//ｉ番目のポリゴンの、ｊ番目の頂点の法線をゲット
			vertices[index].normal = XMVectorSet((float)Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);
		}
	}

	HRESULT hr;
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;


	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
	}
}

void Fbx::InitIndex(fbxsdk::FbxMesh* mesh)
{
	pIndexBuffer_ = new ID3D11Buffer * [materialCount_];

	indexCount_ = new int [materialCount_];

	vector<int> index(polygonCount_ * 3);
	for (int i = 0; i < materialCount_; i++)
	{
		int count = 0;

		//全ポリゴン
		for (DWORD poly = 0; poly < polygonCount_; poly++)
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly);

			if (mtlId == i)
			{
				//3頂点分
				for (DWORD vertex = 0; vertex < 3; vertex++)
				{
					index[count] = mesh->GetPolygonVertex(poly, vertex);
					count++;
				}
			}
		}
		indexCount_[i] = count;
		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * count;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = index.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		HRESULT hr;
		hr = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
		if (FAILED(hr))
		{
			MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		}
	}
}

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
	//pMaterialList_ = new MATERIAL[materialCount_];

	//for (int i = 0; i < materialCount_; i++)
	//{
	//	//i番目のマテリアル情報を取得
	//	FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
	//	FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

	//	//テクスチャ情報
	//	FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	//	//テクスチャの数
	//	int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

	//		FbxDouble3  ambient  = FbxDouble3(0, 0, 0);
	//		FbxDouble3	specular = FbxDouble3(0, 0, 0);
	//		FbxDouble3  diffuse  = FbxDouble3(0, 0, 0);
	//		ambient = pPhong->Ambient;
	//		diffuse = pPhong->Diffuse;

	//	pMaterialList_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
	//	pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
	//	pMaterialList_[i].specular = XMFLOAT4(0, 0, 0, 0);
	//	pMaterialList_[i].shininess = 0;

	//	//Maya側でphongつかった？lambert？ってやつを探す

	//	if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	//	{
	//		specular = pPhong->Specular;
	//		pMaterialList_[i].specular = XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], (float)specular[3]);
	//		pMaterialList_[i].shininess = (float)pPhong->Shininess;
	//	}
	//}

	pMaterialList_ = new MATERIAL[materialCount_];

	for (int i = 0; i < materialCount_; i++)
	{
		//i番目のマテリアル情報を取得
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;
		FbxDouble3 diffuse = pPhong->Diffuse;
		FbxDouble3 ambient = pPhong->Ambient;
		pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		pMaterialList_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
		pMaterialList_[i].specular = XMFLOAT4(0, 0, 0, 0);	//とりあえずハイライトは黒
		pMaterialList_[i].shininess = 1;


		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
			FbxDouble3 specular = pPhong->Specular;
			pMaterialList_[i].specular = XMFLOAT4{ (float)specular[0],(float)specular[1] ,(float)specular[2],1.0f };
			FbxDouble shininess = pPhong->Shininess;
			pMaterialList_[i].shininess = (float)shininess;
		}

		//テクスチャ情報
		FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

		//テクスチャの数
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

		//テクスチャあり
		if (fileTextureCount != 0)
		{
			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();
			//ファイル名+拡張だけにする
			char name[_MAX_FNAME];	//ファイル名
			char ext[_MAX_EXT];	//拡張子
			_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
			wsprintf(name, "%s%s", name, ext);

			//ファイルからテクスチャ作成
			pMaterialList_[i].pTexture = new Texture;
			pMaterialList_[i].pTexture->Load(name);
		}

		//テクスチャ無し
		else
		{
			pMaterialList_[i].pTexture = nullptr;
			//マテリアルの色
			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i);
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		}

		//ノーマルテクスチャ
		{
			//テクスチャ情報
			FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);

			//テクスチャの数数
			int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

			//テクスチャあり
			if (fileTextureCount)
			{
				FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
				const char* textureFilePath = textureInfo->GetRelativeFileName();

				//ファイル名+拡張だけにする
				char name[_MAX_FNAME];	//ファイル名
				char ext[_MAX_EXT];	//拡張子
				_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
				wsprintf(name, "%s%s", name, ext);

				//ファイルからテクスチャ作成
				pMaterialList_[i].pNormalTexture = new Texture;
				HRESULT hr = pMaterialList_[i].pNormalTexture->Load(name);
				assert(hr == S_OK);
			}
			//テクスチャ無し
			else
			{
				pMaterialList_[i].pNormalTexture = nullptr;
				//マテリアルの色
			}
		}

	}


}

void Fbx::IntConstantBuffer()
{
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
	}
}

void Fbx::SetBufferToPipeline(Transform transform)
{
	for (int i = 0; i < materialCount_; i++) {

	CONSTANT_BUFFER cb;
	cb.matW = XMMatrixTranspose(transform.GetWorldMatrix());
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());
	cb.diffuseColor = pMaterialList_[i].diffuse;
	cb.ambientColor = pMaterialList_[i].ambient;
	cb.specularColor = pMaterialList_[i].specular;
	cb.shininess = pMaterialList_[i].shininess;
	cb.hasTexture = pMaterialList_[i].pTexture != nullptr;
	cb.hasNormal = pMaterialList_[i].pNormalTexture != nullptr;
	cb.scrollX = sin(scrollValX_);
	cb.scrollY = sin(scrollValY_);

	Direct3D::pContext_->UpdateSubresource(pConstantBuffer_, 0, NULL, &cb, 0, 0);


		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//再開

	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);
		//コンスタントバッファ
		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

		if (pMaterialList_[i].pTexture) {
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}

		if (pMaterialList_[i].pNormalTexture)
		{
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pNormalTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(1, 1, &pSRV);

		}

		/*ID3D11ShaderResourceView* pSRVToon = pToonTex->GetSRV();
		Direct3D::pContext_->PSSetShaderResources(1, 1, &pSRVToon);*/
		Direct3D::pContext_->DrawIndexed(polygonCount_ * 3, 0, 0);
	}
}

void Fbx::Release()
{
}