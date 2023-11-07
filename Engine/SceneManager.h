
#include "GameObject.h"

enum SCENE_ID
{
	SCENE_ID_TEST = 0,
	SCENE_ID_PLAY,
};
class SceneManager : public GameObject
{
private:
	SCENE_ID currentSceneID;
	SCENE_ID nextSceneID;

public:
	//�R���X�g���N�^
	//�����Fparent 
	SceneManager(GameObject* parent);

	~SceneManager();
	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�J��
	void Release() override;

	void ChangeScene(SCENE_ID _next);
};