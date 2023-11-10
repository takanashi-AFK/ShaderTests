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
	hModel_ = Model::Load("Assets/BoxDefault.fbx");
	pSprite = new Sprite();
	pSprite->Initialize();
}

void Stage::Update()
{
	transform_.rotate_.y += 0.75;
}

void Stage::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);

	spTrans.scale_ = { 0.75 ,0.75 ,0.75 };
	pSprite->Draw(spTrans);
}

void Stage::Release()
{
}
