#pragma once
#include "Engine/GameObject.h"
#include "Engine/Sprite.h"
class Stage : public GameObject
{
private:
	int hModelG;
	int hModelB;
	int hModelA;
	Transform arrowTrans;
	Transform ballTrans;
	Sprite* pSprite;
	Transform spTrans;
	XMFLOAT4 lightPos;

public:
	//コンストラクタ
	//引数：parent 
	Stage(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	XMFLOAT4 GetLightPos;
};