#include "RootJob.h"
#include"SceneManager.h"
#include"../PlayScene.h"

RootJob::RootJob(GameObject* parent)
	:GameObject(parent, "RootJob")
	//木構造の一番上を示すため、parentにはnullが入る
{
}

RootJob::~RootJob()
{
}



void RootJob::Initialize()
{
	Instantiate<SceneManager>(this);
	//SceneManagerを起動するとSceneManagerのInitializeが呼ばれる
	//SceneManagerのInitializeでは、何かしらのSceneをInstantiateしてる
	//RootJob->SceneManager->Sceneって順番
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
