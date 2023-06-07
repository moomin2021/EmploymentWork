#include "TemplateScene.h"

TemplateScene::TemplateScene() :
	key_(nullptr),
	camera_(nullptr)
{
}

TemplateScene::~TemplateScene()
{
	delete camera_;
}

void TemplateScene::Initialize()
{
	// キーボード入力インスタンス取得
	key_ = Key::GetInstance();

	// カメラ
	camera_ = new Camera();
	camera_->eye_ = { 0.0f, 10.0f, -30.0f };
}

void TemplateScene::Update()
{
	// カメラの更新
	camera_->Update();
}

void TemplateScene::Draw()
{
}