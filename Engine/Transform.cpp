#include "Transform.h"

Transform::Transform(): matTranslate_(XMMatrixIdentity()), matRotate_(XMMatrixIdentity()), matScale_(XMMatrixIdentity()), position_({0,0,0}), rotate_({ 0,0,0 }), scale_({1,1,1 }),pParent_(nullptr)
{
}

Transform::~Transform()
{
}

void Transform::Calclation()
{
	//XMMatrixTranslation�́A�w�肳�ꂽ�l�ŕϊ��s����쐬����
	//�����ł́AXMFLOAT3�̃����o�ϐ�position_�̒l�ō쐬���Ă���
	matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z);
	XMMATRIX rotateX, rotateY, rotateZ;

	//XMMatrixRotation �C�ӂ̎��𒆐S�ɉ�]����}�g���b�N�X���쐬���܂��B
	//�l�̓��W�A���Ŏw�肵�Ȃ���΂����Ȃ����߁Adegree�Ŏw�肳�ꂽrotate_��XMConvertToRadians�ŕϊ�����
	rotateX = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(rotate_.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(rotate_.z));
	matRotate_ = rotateZ * rotateX * rotateY;

	//�����܂�܁A�w�肳�ꂽ�l����g�k�s����쐬����
	matScale_ = XMMatrixScaling(scale_.x,scale_.y,scale_.z);
}

XMMATRIX Transform::GetWorldMatrix()
{
	if (pParent_){//�e����������
		return matScale_ * matRotate_ * matTranslate_ * pParent_->GetWorldMatrix();
	}
	return matScale_ * matRotate_ * matTranslate_;
}

XMMATRIX Transform::GetNormalMatrix()
{
	return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}
