#pragma once
#include "BaseScene.h"// -> 各シーンのベース
#include "Key.h"// -> キーボード入力

// シーン列挙型
enum SCENE {
	SCENE1,
};

class SceneManager {
	// --メンバ変数-- //
public:

private:
	// キーボード入力クラス
	Key* key_;

	// 現在のシーン
	BaseScene* nowScene_;

	// --メンバ関数-- //
public:
	// インスタンス取得
	static SceneManager* GetInstance();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

private:
	// コンストラクタ
	SceneManager();

	// デストラクタ
	~SceneManager();

	// シーン切り替え
	void ChangeScene(int changeSceneNum);

	// --禁止-- //
public:
	// コピーコンストラクタ
	SceneManager(const SceneManager& instance) = delete;

	// 代入演算子
	SceneManager& operator=(const SceneManager& instance) = delete;
};