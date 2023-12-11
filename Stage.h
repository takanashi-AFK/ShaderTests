#pragma once
#include "Engine/GameObject.h"
#include "Engine/Sprite.h"
class Stage : public GameObject
{
private:
	int hModelG;
	int hModelB;
	int hModelA;

	int hModelLightPos;
	Transform arrowTrans;
	Transform ballTrans;
	Transform lightPosTrans;
	Sprite* pSprite;
	Transform spTrans;
	XMFLOAT4 lightPos;

	ID3D11Buffer* pCBStageScene_;


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

	void InitConstantBuffer();
};