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
	
	hModelG = Model::Load("Assets/Ground.fbx");
	hModelB = Model::Load("Assets/Sphere.fbx");	
	hModelA = Model::Load("Assets/donut.fbx");
	hModelLightPos = Model::Load("Assets/Sphere.fbx");
	arrowTrans.scale_ = { 0.3,0.3,0.3 };
	arrowTrans.rotate_.y = 0;
	arrowTrans.position_.y = 1.25;
	arrowTrans.position_.z = -2;
	ballTrans.position_.y = 2.5;
	transform_.scale_ = { 7,7,7 };
	lightPosTrans.scale_ = { 0.2,0.2,0.2 };

	lightPos = { 0,0,0,0 };

	InitConstantBuffer();

	
	//pSprite = new Sprite();
	//pSprite->Initialize();
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

	LightPosController::SetLightPosition(lightPos);

	ballTrans.rotate_.y += 0.8;

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

	Model::SetTransform(hModelG, transform_);
	Model::Draw(hModelG);
	
	Model::SetTransform(hModelA, arrowTrans);
	Model::Draw(hModelA);
	
	Model::SetTransform(hModelB, ballTrans);
	Model::Draw(hModelB);
	
	Model::SetTransform(hModelLightPos, lightPosTrans);
	Model::Draw(hModelLightPos);

	//spTrans.scale_ = { 0.75 ,0.75 ,0.75 };
	//pSprite->Draw(spTrans);
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
