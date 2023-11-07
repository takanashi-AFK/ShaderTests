#include "Transform.h"

Transform::Transform(): matTranslate_(XMMatrixIdentity()), matRotate_(XMMatrixIdentity()), matScale_(XMMatrixIdentity()), position_({0,0,0}), rotate_({ 0,0,0 }), scale_({1,1,1 }),pParent_(nullptr)
{
}

Transform::~Transform()
{
}

void Transform::Calclation()
{
	//XMMatrixTranslationは、指定された値で変換行列を作成する
	//ここでは、XMFLOAT3のメンバ変数position_の値で作成している
	matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z);
	XMMATRIX rotateX, rotateY, rotateZ;

	//XMMatrixRotation 任意の軸を中心に回転するマトリックスを作成します。
	//値はラジアンで指定しなければいけないため、degreeで指定されたrotate_をXMConvertToRadiansで変換する
	rotateX = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(rotate_.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(rotate_.z));
	matRotate_ = rotateZ * rotateX * rotateY;

	//もうまんま、指定された値から拡縮行列を作成する
	matScale_ = XMMatrixScaling(scale_.x,scale_.y,scale_.z);
}

XMMATRIX Transform::GetWorldMatrix()
{
	if (pParent_){//親があったら
		return matScale_ * matRotate_ * matTranslate_ * pParent_->GetWorldMatrix();
	}
	return matScale_ * matRotate_ * matTranslate_;
}

XMMATRIX Transform::GetNormalMatrix()
{
	return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}
