#include "RootJob.h"
#include"SceneManager.h"
#include"../PlayScene.h"

RootJob::RootJob(GameObject* parent)
	:GameObject(parent, "RootJob")
	//�؍\���̈�ԏ���������߁Aparent�ɂ�null������
{
}

RootJob::~RootJob()
{
}



void RootJob::Initialize()
{
	Instantiate<SceneManager>(this);
	//SceneManager���N�������SceneManager��Initialize���Ă΂��
	//SceneManager��Initialize�ł́A���������Scene��Instantiate���Ă�
	//RootJob->SceneManager->Scene���ď���
}

void RootJob::Update()
{
}

void RootJob::Draw()
{
}

void RootJob::Release()
{
}
