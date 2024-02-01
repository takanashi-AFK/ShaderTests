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
	hModelLightPos = Model::Load("Assets/Dice.fbx");
	hModelA = Model::Load("Assets/Water/Water.FBX");

	lightPos = { 0,0,0,0 };

	InitConstantBuffer();

	Direct3D::SetDepthBafferWriteEnable(false);


}

void Stage::Update()
{
	//�����̈ʒu��WASD�Ƃ��Ŏw��
	lightPos = LightPosController::GetLightPosition();
	//Model����LightPos�����Ă���
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

	lightPosTrans.position_ = { lightPos.x,lightPos.y, lightPos.z };

	CBUFF_STAGESCENE cbStage;

	lightPosTrans.rotate_.z += 0.2;
	lightPosTrans.rotate_.y += 0.2;

	//cbStage.lightPosition = LightPosController::GetLightPosition();
	cbStage.lightPosition = lightPos;
	XMStoreFloat4(&cbStage.eyepos, Camera::GetPosition());

	Direct3D::pContext_->UpdateSubresource(pCBStageScene_, 0, NULL, &cbStage, 0, 0);

	Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCBStageScene_);	//���_�V�F�[�_�[�p	
	Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pCBStageScene_);	//�s�N�Z���V�F�[�_�[�p

}

void Stage::Draw()
{
	Model::SetTransform(hModelA, transform_);
	Model::Draw(hModelA);
	Model::SetTransform(hModelLightPos, lightPosTrans);
	Model::Draw(hModelLightPos);
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

	// �R���X�^���g�o�b�t�@�̍쐬
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pCBStageScene_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
	}

}
