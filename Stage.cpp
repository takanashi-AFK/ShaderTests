#include "Stage.h"
#include"Engine/SceneManager.h"
#include"Engine/Input.h"
#include"Engine/Model.h"
#include "LightPosController.h"


Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage")
{
}

struct CBUFF_STAGESCENE
{
	XMFLOAT4	lightPosition;
	XMFLOAT4	eyepos;
};

void Stage::Initialize()
{
	hModelLightPos = Model::Load("Assets/donut.fbx");
	hModelA = Model::Load("Assets/Water/Water.FBX");

	lightPos = { 0,0,0,0 };

	InitConstantBuffer();

	Direct3D::SetDepthBafferWriteEnable(true);
	pq = new Quad;
	pq->Initialize();

	arrowTrans.position_.y = 4;
	arrowTrans.scale_ = { 3,3,3 };
}

void Stage::Update()
{
	//光源の位置をWASDとかで指定
	lightPos = LightPosController::GetLightPosition();
	//ModelからLightPosもってくる
	if (Input::IsKey(DIK_W))
	{
		lightPos.y += 0.03;

	}

	if (Input::IsKey(DIK_A))
	{
		lightPos.x -= 0.03;

	}

	if (Input::IsKey(DIK_S))
	{
		lightPos.y -= 0.03;

	}

	if (Input::IsKey(DIK_D))
	{
		lightPos.x += 0.03;

	}

	if (Input::IsKey(DIK_UP))
	{
		lightPos.z += 0.03;
	}

	if (Input::IsKey(DIK_DOWN))
	{
		lightPos.z -= 0.03;
	}

	if (Input::IsKey(DIK_LEFT))
	{
		lightPosTrans.rotate_.y -= 0.5;
	}
	if (Input::IsKey(DIK_RIGHT))
	{
		lightPosTrans.rotate_.y += 0.5;
	}

	LightPosController::SetLightPosition(lightPos);

	lightPosTrans.position_ = { lightPos.x,lightPos.y, lightPos.z };
	CBUFF_STAGESCENE cbStage;

	//cbStage.lightPosition = LightPosController::GetLightPosition();
	cbStage.lightPosition = lightPos;
	XMStoreFloat4(&cbStage.eyepos, Camera::GetPosition());

	Direct3D::pContext_->UpdateSubresource(pCBStageScene_, 0, NULL, &cbStage, 0, 0);

	Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCBStageScene_);	//頂点シェーダー用	
	Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pCBStageScene_);	//ピクセルシェーダー用
}

void Stage::Draw()
{
	Direct3D::SetShader(SHADER_2D);
	Model::SetTransform(hModelA, transform_);
	Model::Draw(hModelA);
	Model::SetTransform(hModelLightPos, lightPosTrans);
	Model::Draw(hModelLightPos);

	pq->Draw(arrowTrans);
}

void Stage::Release()
{
}

void Stage::InitConstantBuffer()
{
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CBUFF_STAGESCENE);
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pCBStageScene_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
	}

}
