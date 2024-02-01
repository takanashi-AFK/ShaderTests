#include "Fbx.h"
#include"Texture.h"
#include"../LightPosController.h"
using namespace std;
Fbx::Fbx():pVertexBuffer_(nullptr),pIndexBuffer_(nullptr), pConstantBuffer_(nullptr),vertexCount_(0),polygonCount_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{
	//�}�l�[�W���𐶐�
	FbxManager* pFbxManager = FbxManager::Create();

	//�C���|�[�^�[�𐶐�
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//�V�[���I�u�W�F�N�g��FBX�t�@�C���̏��𗬂�����
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	//���b�V�������擾
	FbxNode* rootNode = pFbxScene->GetRootNode();
	FbxNode* pNode = rootNode->GetChild(0);
	FbxMesh* mesh = pNode->GetMesh();

	//�e���̌����擾
	vertexCount_ = mesh->GetControlPointsCount();	//���_�̐�
	polygonCount_ = mesh->GetPolygonCount();	//�|���S���̐�
	materialCount_ = pNode->GetMaterialCount();


	//���݂̃J�����g�f�B���N�g�����擾
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//������fileName����f�B���N�g���������擾
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
	//�J�����g�f�B���N�g����ύX
	SetCurrentDirectory(dir);


	InitVertex(mesh);	//���_�o�b�t�@����
	InitIndex(mesh);	//�C���f�b�N�X�o�b�t�@����
	IntConstantBuffer();	//�R���X�^���g�o�b�t�@����
	InitMaterial(pNode);

	//�I�������߂�
	SetCurrentDirectory(defaultCurrentDir);


	//�}�l�[�W�����
	pFbxManager->Destroy();

	pToonTex = new Texture;

	pToonTex->Load("Assets/toonTexture.png");
	return S_OK;
}

void Fbx::Draw(Transform& transform)
{
	Direct3D::SetShader(SHADER_NORMAL);
	transform.Calclation();

	//���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�R���X�^���g�o�b�t�@���p�C�v���C���ɃZ�b�g
	SetBufferToPipeline(transform);

}


void Fbx::InitVertex(fbxsdk::FbxMesh* mesh)
{
	//���_��������z��
	VERTEX* vertices = new VERTEX[vertexCount_];

	//�S�|���S��
	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3���_��
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//���ׂ钸�_�̔ԍ�
			//1�̃|���S�����\�����钸�_�̃C���f�b�N�X�ԍ��̃Z�b�g�𓾂�֐�
			int index = mesh->GetPolygonVertex(poly, vertex);

			//���_�̈ʒu
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//���_��UV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);
			//���_�̖@��
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//���Ԗڂ̃|���S���́A���Ԗڂ̒��_�̖@�����Q�b�g
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
		MessageBox(NULL, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
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

		//�S�|���S��
		for (DWORD poly = 0; poly < polygonCount_; poly++)
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly);

			if (mtlId == i)
			{
				//3���_��
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
			MessageBox(NULL, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		}
	}
}

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
	//pMaterialList_ = new MATERIAL[materialCount_];

	//for (int i = 0; i < materialCount_; i++)
	//{
	//	//i�Ԗڂ̃}�e���A�������擾
	//	FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
	//	FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

	//	//�e�N�X�`�����
	//	FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	//	//�e�N�X�`���̐�
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

	//	//Maya����phong�������Hlambert�H���Ă��T��

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
		//i�Ԗڂ̃}�e���A�������擾
		FbxSurfacePhong* pMaterial =static_cast<FbxSurfacePhong*>( pNode->GetMaterial(i));
		FbxDouble3 diffuse = pMaterial->Diffuse;
		FbxDouble3 ambient = pMaterial->Ambient;

		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
			FbxDouble3 specular = pMaterial->Specular;
			FbxDouble shininess = pMaterial->Shininess;
			pMaterialList_[i].specular = XMFLOAT4{ (float)specular[0],(float)specular[1] ,(float)specular[2],1.0f };
			pMaterialList_[i].shininess = (float)shininess;
		}

		pMaterialList_[i].diffuse = XMFLOAT4{ (float)diffuse[0],(float)diffuse[1] ,(float)diffuse[2],1.0f };
		pMaterialList_[i].ambient = XMFLOAT4{ (float)ambient[0],(float)ambient[1] ,(float)ambient[2],1.0f };
		//�e�N�X�`�����
		FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

		//�e�N�X�`���̐���
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

		//�e�N�X�`������
		if (fileTextureCount != 0)
		{
			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();
			//�t�@�C����+�g�������ɂ���
			char name[_MAX_FNAME];	//�t�@�C����
			char ext[_MAX_EXT];	//�g���q
			_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
			wsprintf(name, "%s%s", name, ext);

			//�t�@�C������e�N�X�`���쐬
			pMaterialList_[i].pTexture = new Texture;
			pMaterialList_[i].pTexture->Load(name);
		}

		//�e�N�X�`������
		else
		{
			pMaterialList_[i].pTexture = nullptr;
			//�}�e���A���̐F
			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i);
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
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

	// �R���X�^���g�o�b�t�@�̍쐬
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
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
	cb.isNormal = pMaterialList_[i].pNormalTexture != nullptr;
	cb.isTexture = pMaterialList_[i].pTexture != nullptr;

	Direct3D::pContext_->UpdateSubresource(pConstantBuffer_, 0, NULL, &cb, 0, 0);


		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ

	//���_�o�b�t�@
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
		// �C���f�b�N�X�o�b�t�@�[���Z�b�g
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);
		//�R���X�^���g�o�b�t�@
		Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//���_�V�F�[�_�[�p	
		Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//�s�N�Z���V�F�[�_�[�p

		if (pMaterialList_[i].pTexture) {
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}

		ID3D11ShaderResourceView* pSRVToon = pToonTex->GetSRV();
		Direct3D::pContext_->PSSetShaderResources(1, 1, &pSRVToon);

		Direct3D::pContext_->DrawIndexed(polygonCount_ * 3, 0, 0);
	}
}

void Fbx::Release()
{
}