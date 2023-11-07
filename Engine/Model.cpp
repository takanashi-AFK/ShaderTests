#include "Model.h"
#include"Direct3D.h"
namespace Model
{
	std::vector<ModelData*>modelList;
}

int Model::Load(std::string _filename)
{
	ModelData* pData = new ModelData;
	pData->filename_ = _filename;
	pData->fbx_ = nullptr;
	for (auto e : modelList) {
		if (e->filename_ == _filename)
			pData->fbx_ = e->fbx_;
		break;
	}

	if (pData->fbx_ == nullptr){
	pData->fbx_ = new Fbx;
	pData->fbx_->Load(_filename);
	}
	modelList.push_back(pData);
	return modelList.size() - 1;
}

void Model::SetTransform(int _hModel, Transform _transform)
{
	modelList[_hModel]->transform_ = _transform;
}

void Model::Draw(int _hModel)
{	
	modelList[_hModel]->fbx_->Draw(modelList[_hModel]->transform_);
}

void Model::Release()
{
	bool isReferrd=false;//QÆ‚³‚ê‚Ä‚éH
	for (int i = 0;i<modelList.size();i++)
	{
		for (int j = i + 1; j < modelList.size(); j++)
		{
			if (modelList[i]->fbx_ == modelList[j]->fbx_) {
				isReferrd = true;
				break;
			}
		}
		if (isReferrd == false) {
			SAFE_DELETE(modelList[i]->fbx_);
		}
		SAFE_DELETE(modelList[i]);
	}
}
