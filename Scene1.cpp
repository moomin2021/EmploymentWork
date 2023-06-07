#include "Scene1.h"
#include <json.hpp>
#include <fstream>

void Scene1::LoadJson() {
	// �A�����ăt���p�X�𓾂�
	const std::string fullpath = std::string("Resources/levels/") + "level.json";

	// �t�@�C���X�g���[��
	std::ifstream file;

	// �t�@�C�����J��
	file.open(fullpath);

	// �t�@�C���I�[�v�����s�`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	// JSON�����񂩂�𓀂����f�[�^
	nlohmann::json deserialized;

	// ��
	file >> deserialized;

	// ���������x���G�f�B�^�t�@�C�����`�F�b�N
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();

	// ���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("scene") == 0);

	// ���x���f�[�^�i�[�p�C���X�^���X�𐶐�
	LevelData* levelData = new LevelData();

	// "objects"�̑S�I�u�W�F�N�g�𑖍�
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// ��ʂ��擾
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// �v�f�ǉ�
			levelData->objects.emplace_back(LevelData::ObjectData{});

			// ���ǉ������v�f�̎Q�Ƃ𓾂�
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				// �t�@�C����
				objectData.fileName = object["file_name"];
			}

			// �������X�t�H�[���̃p�����[�^�ǂݍ���
			nlohmann::json& transform = object["transform"];

			// ���s�ړ�
			objectData.translation.x = (float)transform["translation"][1];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = (float)-transform["translation"][0];

			// ��]�p
			objectData.rotation.x = (float)-transform["rotation"][1];
			objectData.rotation.y = (float)-transform["rotation"][2];
			objectData.rotation.z = (float)transform["rotation"][0];

			// �X�P�[�����O
			objectData.scaling.x = (float)transform["scaling"][1];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][0];
		}

		// �I�u�W�F�N�g�������ċA�֐��ɂ܂Ƃ߁A�ċA�ďo�Ŏ}�𑖍�����
		if (object.contains("children")) {

		}
	}

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {
		//// �t�@�C��������o�^�ς݃��f��������
		//Model* model = nullptr;

		//decltype(models)::iterator it = models.find(objectData.fileName);

		//if (it != models.end()) { model = it->second; }

		// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		Object3D* newObject = Object3D::CreateObject3D(cubeM_);

		// ���W
		newObject->SetPos(objectData.translation);

		// ��]�p
		newObject->SetRot(objectData.rotation);

		// �g�k
		newObject->SetScale(objectData.scaling);

		// �z��ɓo�^
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
	// �L�[�{�[�h���̓C���X�^���X�擾
	key_ = Key::GetInstance();

	// �J����
	camera_ = new Camera();
	camera_->eye_ = { 0.0f, 10.0f, -30.0f };

	// ���f��
	//sphereM_ = Model::CreateModel("sphere");
	cubeM_ = Model::CreateModel("cube");
	floorM_ = Model::CreateModel("floor");

	// �I�u�W�F�N�g
	object_[0] = Object3D::CreateObject3D(cubeM_);
	object_[0]->SetPos({ 3.0f, 1.0f, 0.0f });

	object_[1] = Object3D::CreateObject3D(cubeM_);
	object_[1]->SetPos({ -3.0f, 1.0f, 0.0f });

	object_[2] = Object3D::CreateObject3D(floorM_);
	object_[2]->SetPos({ 0.0f, 0.0f, 0.0f });
	object_[2]->SetScale({ 10.0f, 10.0f, 10.0f });

	// ���C�g����
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

	// �J������ݒ�
	Object3D::SetCamera(camera_);

	// ���C�g��ݒ�
	Object3D::SetLightGroup(lightGroup_);

	// Json�t�@�C����ǂݍ��ݔz�u
	LoadJson();
}

void Scene1::Update()
{
	if (key_->TriggerKey(DIK_0)) {
		int num = 0;
	}

	// �J�����ړ�
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

	// �J�����̍X�V
	camera_->Update();

	// ���C�g�̍X�V
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