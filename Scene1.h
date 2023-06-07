#pragma once
#include "BaseScene.h"
#include "Model.h"
#include "Object3D.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Key.h"

#include <map>

struct LevelData {

	// オブジェクト1個分のデータ
	struct ObjectData {
		std::string fileName;
		Float3 translation;
		Float3 rotation;
		Float3 scaling;
	};

	// オブジェクトコンテナ
	std::vector<ObjectData> objects;
};

class Scene1 : public BaseScene
{
	// --メンバ変数-- //
private:
	// キーボード入力
	Key* key_;

	// カメラ
	Camera* camera_;

	// ライト
	LightGroup* lightGroup_;

	// モデル
	Model* sphereM_;
	Model* cubeM_;
	Model* floorM_;

	// オブジェクト
	Object3D* object_[3];

	std::map<std::string, Model*> models;
	std::vector<Object3D*> objects;

	// --メンバ関数-- //
public:
	// コンストラクタ
	Scene1();

	// デストラクタ
	~Scene1();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

private:
	void LoadJson();
};

