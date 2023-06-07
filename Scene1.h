#pragma once
#include "BaseScene.h"
#include "Model.h"
#include "Object3D.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Key.h"

#include <map>

struct LevelData {

	// �I�u�W�F�N�g1���̃f�[�^
	struct ObjectData {
		std::string fileName;
		Float3 translation;
		Float3 rotation;
		Float3 scaling;
	};

	// �I�u�W�F�N�g�R���e�i
	std::vector<ObjectData> objects;
};

class Scene1 : public BaseScene
{
	// --�����o�ϐ�-- //
private:
	// �L�[�{�[�h����
	Key* key_;

	// �J����
	Camera* camera_;

	// ���C�g
	LightGroup* lightGroup_;

	// ���f��
	Model* sphereM_;
	Model* cubeM_;
	Model* floorM_;

	// �I�u�W�F�N�g
	Object3D* object_[3];

	std::map<std::string, Model*> models;
	std::vector<Object3D*> objects;

	// --�����o�֐�-- //
public:
	// �R���X�g���N�^
	Scene1();

	// �f�X�g���N�^
	~Scene1();

	// ����������
	void Initialize();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

private:
	void LoadJson();
};

