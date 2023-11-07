
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
	//コンストラクタ
	//引数：parent 
	SceneManager(GameObject* parent);

	~SceneManager();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void ChangeScene(SCENE_ID _next);
};