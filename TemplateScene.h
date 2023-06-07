#pragma once
#include "BaseScene.h"
#include "Model.h"
#include "Object3D.h"
#include "Camera.h"
#include "Key.h"

class TemplateScene : public BaseScene
{
	// --メンバ変数-- //
private:
	// キーボード入力
	Key* key_;

	// カメラ
	Camera* camera_;

	// --メンバ関数-- //
public:
	// コンストラクタ
	TemplateScene();

	// デストラクタ
	~TemplateScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();
};

