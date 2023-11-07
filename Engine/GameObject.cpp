#include "GameObject.h"

GameObject::GameObject() :isDead_(false),pParent_(nullptr)
{
}

GameObject::GameObject(GameObject* parent, const std::string& name):pParent_(parent), objectName_(name), isDead_(false)
{
	if (parent != nullptr)
		this->transform_.pParent_ = &(parent->transform_);
}

GameObject::~GameObject()
{
}

void GameObject::DrawSub()
{
	Draw();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
	{
		(*itr)->DrawSub();
	}
}

void GameObject::InitializeSub()
{
	Initialize();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
	{
		(*itr)->InitializeSub();
	}
}

void GameObject::UpdateSub()
{
	Update();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
	{
		(*itr)->UpdateSub();
	}
	for (auto itr = childList_.begin(); itr != childList_.end();)
	{
		if ((*itr)->isDead_) {
			(*itr)->ReleaseSub();
			delete *itr;
			itr = childList_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void GameObject::ReleaseSub()
{
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
	{
		(*itr)->ReleaseSub();
		delete *itr;
	}
	Release();
}

void GameObject::KillMe()
{
	isDead_ = true;
}

GameObject* GameObject::FindChildObject(std::string _objName)
{
	if (_objName == this->objectName_) {
		return this;
	}
	else {
		for (auto itr:childList_) {
			GameObject* obj  = itr->FindChildObject(_objName);
			if (obj != nullptr)
			{
				return obj;
			}
		}
	}
}

GameObject* GameObject::GetRootJob()
{
	if(pParent_ ==  nullptr)
	return this;

	return pParent_->GetRootJob();
}

void GameObject::SetPosition(XMFLOAT3 _pos)
{
	transform_.position_ = _pos;

}

GameObject* GameObject::FindObject(std::string _objName)
{

	return GetRootJob()->FindChildObject(_objName);
}

void GameObject::SetScale(XMFLOAT3 _scl)
{
	transform_.scale_ = _scl;
}
