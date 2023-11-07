#include "TestObj.h"
#include"Engine/Fbx.h"
#include"Engine/Input.h"
#include"Engine/Model.h"
//�R���X�g���N�^
TestObj::TestObj(GameObject* parent)
	: GameObject(parent, "TestObj"), pFbx1(nullptr), hModel_(-1)
{
}

//������
void TestObj::Initialize()
{
	hModel_ = Model::Load("Assets/odden.fbx");
	assert(hModel_ >= 0);
	transform_.scale_.x = 0.2f;
	transform_.scale_.y = 0.2f;
	transform_.scale_.z = 0.2f;

	transform_.rotate_.x = 90;

}

//�X�V
void TestObj::Update()
{
	if (Input::IsKey(DIK_K))
	{
		KillMe();
	}
	transform_.position_.z += 0.5f;

	if (transform_.position_.z >= 50)
	{
		KillMe();
	}
}

//�`��
void TestObj::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

//�J��
void TestObj::Release()
{

}