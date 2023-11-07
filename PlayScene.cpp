#include "PlayScene.h"
#include"Engine/Fbx.h"
#include"TestObj.h"
#include"Stage.h"
//コンストラクタ
PlayScene::PlayScene(GameObject* parent)
	: GameObject(parent, "PlayScene"),pFbx(nullptr)
{
}

//初期化
void PlayScene::Initialize()
{
	Instantiate<Stage>(this);
	
}

//更新
void PlayScene::Update()
{
}

//描画
void PlayScene::Draw()
{
}

//開放
void PlayScene::Release()
{
}