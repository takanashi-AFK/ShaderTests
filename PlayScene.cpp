#include "PlayScene.h"
#include"Engine/Fbx.h"
#include"TestObj.h"
#include"Stage.h"
//�R���X�g���N�^
PlayScene::PlayScene(GameObject* parent)
	: GameObject(parent, "PlayScene"),pFbx(nullptr)
{
}

//������
void PlayScene::Initialize()
{
	Instantiate<Stage>(this);
	
}

//�X�V
void PlayScene::Update()
{
}

//�`��
void PlayScene::Draw()
{
}

//�J��
void PlayScene::Release()
{
}