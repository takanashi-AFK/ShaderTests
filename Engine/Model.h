#pragma once

#include<string>
#include"Transform.h"
#include<vector>
#include"Fbx.h"
namespace Model
{
	struct ModelData
	{
		Fbx* fbx_;
		std::string filename_;
		Transform	transform_;
	};
		int Load(std::string _filename);
		void SetTransform(int _hModel, Transform _transform);
		void Draw(int _hModel);
		void Release();
}
