#include "SceneManager.h"
#include"../TestScene.h"
#include"../PlayScene.h"
#include"Direct3D.h"
#include"Model.h"
SceneManager::SceneManager(GameObject* parent)
	:GameObject(parent, "SceneManager")
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	currentSceneID = SCENE_ID_PLAY;
	nextSceneID = currentSceneID;

	Instantiate<PlayScene>(this);
}

void SceneManager::Update()
{
	if (currentSceneID != nextSceneID)
	{
		auto scene = childList_.begin();
		SAFE_DELETE(*scene)
		childList_.clear();

		Model::Release();

		switch (nextSceneID)
		{
		case SCENE_ID_TEST:
			Instantiate<TestScene>(this);
			break;
		case SCENE_ID_PLAY:
			Instantiate<PlayScene>(this);
			break;
		}

		currentSceneID = nextSceneID;

	}
}

void SceneManager::Draw()
{
}

void SceneManager::Release()
{
}

void SceneManager::ChangeScene(SCENE_ID _next)
{

	nextSceneID = _next;
}
