#include "Stage.h"
#include"Engine/SceneManager.h"
#include"Engine/Input.h"
#include"Engine/Model.h"


Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage")
{
}

void Stage::Initialize()
{
	
	hModelG = Model::Load("Assets/Ground.fbx");
	hModelB = Model::Load("Assets/Sphere.fbx");
	hModelA = Model::Load("Assets/Arrow.fbx");
	arrowTrans.rotate_.y = -90;
	ballTrans.position_.y = 2.5;
	
	//pSprite = new Sprite();
	//pSprite->Initialize();
}

void Stage::Update()
{
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
