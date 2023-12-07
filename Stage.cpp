#include "Stage.h"
#include"Engine/SceneManager.h"
#include"Engine/Input.h"
#include"Engine/Model.h"
#include "LightPosController.h"


Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage")
{
}

void Stage::Initialize()
{
	
	hModelG = Model::Load("Assets/Ground.fbx");
	hModelB = Model::Load("Assets/Sphere.fbx");	
	hModelA = Model::Load("Assets/donut.fbx");
	arrowTrans.scale_ = { 0.3,0.3,0.3 };
	arrowTrans.rotate_.y = 0;
	arrowTrans.position_.y = 1.25;
	arrowTrans.position_.z = -2;
	ballTrans.position_.y = 2.5;
	transform_.scale_ = { 7,7,7 };
	
	//pSprite = new Sprite();
	//pSprite->Initialize();
}

void Stage::Update()
{
	//ŒõŒ¹‚ÌˆÊ’u‚ðWASD‚Æ‚©‚ÅŽw’è
	lightPos = LightPosController::GetLightPosition();
	//Model‚©‚çLightPos‚à‚Á‚Ä‚­‚é
	if (Input::IsKey(DIK_W))
	{
		lightPos.z += 0.5;

	}

	if (Input::IsKey(DIK_A))
	{
		lightPos.x += 0.5;

	}

	if (Input::IsKey(DIK_S))
	{
		lightPos.z -= 0.5;

	}

	if (Input::IsKey(DIK_D))
	{
		lightPos.x -= 0.5;

	}

	LightPosController::SetLightPosition(lightPos);

	ballTrans.rotate_.y += 0.8;
}

void Stage::Draw()
{

	Model::SetTransform(hModelG, transform_);
	Model::Draw(hModelG);
	
	Model::SetTransform(hModelA, arrowTrans);
	Model::Draw(hModelA);
	
	Model::SetTransform(hModelB, ballTrans);
	Model::Draw(hModelB);

	//spTrans.scale_ = { 0.75 ,0.75 ,0.75 };
	//pSprite->Draw(spTrans);
}

void Stage::Release()
{
}
