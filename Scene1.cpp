#include "Scene1.h"
#include <json.hpp>
#include <fstream>

void Scene1::LoadJson() {
	// 連結してフルパスを得る
	const std::string fullpath = std::string("Resources/levels/") + "level.json";

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);

	// ファイルオープン失敗チェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	// 正しいレベルエディタファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();

	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});

			// 今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.fileName = object["file_name"];
			}

			// ｔランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];

			// 平行移動
			objectData.translation.x = (float)transform["translation"][1];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = (float)-transform["translation"][0];

			// 回転角
			objectData.rotation.x = (float)-transform["rotation"][1];
			objectData.rotation.y = (float)-transform["rotation"][2];
			objectData.rotation.z = (float)transform["rotation"][0];

			// スケーリング
			objectData.scaling.x = (float)transform["scaling"][1];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][0];
		}

		// オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {

		}
	}

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		//// ファイル名から登録済みモデルを検索
		//Model* model = nullptr;

		//decltype(models)::iterator it = models.find(objectData.fileName);

		//if (it != models.end()) { model = it->second; }

		// モデルを指定して3Dオブジェクトを生成
		Object3D* newObject = Object3D::CreateObject3D(cubeM_);

		// 座標
		newObject->SetPos(objectData.translation);

		// 回転角
		newObject->SetRot(objectData.rotation);

		// 拡縮
		newObject->SetScale(objectData.scaling);

		// 配列に登録
		objects.push_back(newObject);
	}
}

Scene1::Scene1() :
	key_(nullptr),
	camera_(nullptr),
	lightGroup_(nullptr),
	sphereM_(nullptr),
	object_{}
{
}

Scene1::~Scene1()
{
	delete camera_;
	delete lightGroup_;
	delete sphereM_;
	delete cubeM_;
	delete floorM_;
	delete object_[0];
	delete object_[1];
	delete object_[2];
}

void Scene1::Initialize()
{
	// キーボード入力インスタンス取得
	key_ = Key::GetInstance();

	// カメラ
	camera_ = new Camera();
	camera_->eye_ = { 0.0f, 10.0f, -30.0f };

	// モデル
	//sphereM_ = Model::CreateModel("sphere");
	cubeM_ = Model::CreateModel("cube");
	floorM_ = Model::CreateModel("floor");

	// オブジェクト
	object_[0] = Object3D::CreateObject3D(cubeM_);
	object_[0]->SetPos({ 3.0f, 1.0f, 0.0f });

	object_[1] = Object3D::CreateObject3D(cubeM_);
	object_[1]->SetPos({ -3.0f, 1.0f, 0.0f });

	object_[2] = Object3D::CreateObject3D(floorM_);
	object_[2]->SetPos({ 0.0f, 0.0f, 0.0f });
	object_[2]->SetScale({ 10.0f, 10.0f, 10.0f });

	// ライト生成
	lightGroup_ = LightGroup::Create();

	lightGroup_->SetDirLightActive(0, true);
	lightGroup_->SetDirLightActive(1, false);
	lightGroup_->SetDirLightActive(2, false);
	lightGroup_->SetPointLightActive(0, true);
	lightGroup_->SetPointLightActive(1, false);
	lightGroup_->SetPointLightActive(2, false);
	lightGroup_->SetSpotLightActive(0, false);
	lightGroup_->SetSpotLightActive(1, false);
	lightGroup_->SetSpotLightActive(2, false);
	lightGroup_->SetCircleShadowActive(0, false);

	// カメラを設定
	Object3D::SetCamera(camera_);

	// ライトを設定
	Object3D::SetLightGroup(lightGroup_);

	// Jsonファイルを読み込み配置
	LoadJson();
}

void Scene1::Update()
{
	if (key_->TriggerKey(DIK_0)) {
		int num = 0;
	}

	// カメラ移動
	{
		camera_->eye_.x += (key_->PushKey(DIK_D) - key_->PushKey(DIK_A)) * 0.5f;
		camera_->eye_.z += (key_->PushKey(DIK_W) - key_->PushKey(DIK_S)) * 0.5f;
	}

	if (key_->TriggerKey(DIK_E)) {
		object_[0]->SetModel(cubeM_);
	}

	if (key_->TriggerKey(DIK_Q)) {
		object_[0]->SetModel(sphereM_);
	}

	static Float3 pos = { 0.0f, 0.5f, 0.0f };

	pos.x += (key_->PushKey(DIK_RIGHT) - key_->PushKey(DIK_LEFT)) * 0.2f;
	pos.z += (key_->PushKey(DIK_UP) - key_->PushKey(DIK_DOWN)) * 0.2f;

	lightGroup_->SetPointLightPos(0, XMFLOAT3({ pos.x, pos.y, pos.z }));

	static float rota = 0.0f;
	rota += 1.0f;

	object_[0]->SetRot({ 0.0f, rota, 0.0f });
	object_[1]->SetRot({0.0f, rota, 0.0f});

	// カメラの更新
	camera_->Update();

	// ライトの更新
	lightGroup_->Update();
}

void Scene1::Draw()
{
	Object3D::PreDraw();

	for (auto& object : object_) object->Draw();

	for (Object3D* object : objects) {
		object->Draw();
	}
}