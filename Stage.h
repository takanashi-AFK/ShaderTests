#pragma once
#include "Engine/GameObject.h"
#include "Engine/Sprite.h"
#include "Engine/Quad.h"
class Stage : public GameObject
{
private:
	int hModelG;
	int hModelB;
	int hModelA;

	int hModelLightPos;
	float scrollVal_;
	Transform arrowTrans;
	Transform ballTrans;
	Transform lightPosTrans;
	Sprite* pSprite;
	Transform spTrans;
	XMFLOAT4 lightPos;

	ID3D11Buffer* pCBStageScene_;
	Quad* pq;

public:
	//�R���X�g���N�^
	//�����Fparent 
	Stage(GameObject* parent);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�J��
	void Release() override;

	XMFLOAT4 GetLightPos;

	void InitConstantBuffer();
};