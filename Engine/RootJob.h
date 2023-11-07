#pragma once
#include"GameObject.h"
class RootJob:public GameObject
{
private:
public:
	RootJob(GameObject* parent);
	~RootJob();

	void Initialize()override;
	void Update()	 override;
	void Draw()		 override;
	void Release()	 override;
};

