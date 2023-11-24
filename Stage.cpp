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
	hModelA = Model::Load("Assets/arrow2.fbx");
	ballTrans.position_.y = 2.5;
	transform_.scale_ = { 8,8,8 };

	arrowTransX.scale_ = { 0.25,0.25,0.25 };
	arrowTransX.position_.y = 0.25;

	arrowTransY.scale_ = { 0.25,0.25,0.25 };
	arrowTransY.rotate_.y = -90;
	arrowTransY.position_.y = 0.25;

	arrowTransZ.scale_ = { 0.25,0.25,0.25 };
	arrowTransZ.rotate_.z = 90;
	arrowTransZ.position_.y = 0.25;
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
	
	Model::SetTransform(hModelA, arrowTransX);
	Model::Draw(hModelA);

	Model::SetTransform(hModelA, arrowTransY);
	Model::Draw(hModelA);

	Model::SetTransform(hModelA, arrowTransZ);
	Model::Draw(hModelA);
	
	Model::SetTransform(hModelB, ballTrans);
	Model::Draw(hModelB);

	//spTrans.scale_ = { 0.75 ,0.75 ,0.75 };
	//pSprite->Draw(spTrans);
}

void Stage::Release()
{
}
