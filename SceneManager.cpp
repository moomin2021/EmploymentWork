#include "SceneManager.h"
#include "Scene1.h"

// インスタンス取得
SceneManager* SceneManager::GetInstance()
{
	// 関数内static変数として宣言
	static SceneManager instance;
	return &instance;
}

// コンストラクタ
SceneManager::SceneManager() {
	// インスタンス取得
	key_ = Key::GetInstance();

	// 最初のシーン
	nowScene_ = new Scene1();
	nowScene_->Initialize();
}

// デストラクタ
SceneManager::~SceneManager() {
	delete nowScene_;
}

void SceneManager::ChangeScene(int changeSceneNum)
{
	// 現在のシーンを解放処理
	if (nowScene_ != nullptr) {
		delete nowScene_;
	}

	switch (changeSceneNum)
	{
	case SCENE::SCENE1:
		nowScene_ = new Scene1();
		nowScene_->Initialize();
		break;
	}
}

// 更新処理
void SceneManager::Update() {

	if (key_->TriggerKey(DIK_1)) ChangeScene(SCENE::SCENE1);

	nowScene_->Update();
}

// 描画処理
void SceneManager::Draw()
{
	nowScene_->Draw();
}
