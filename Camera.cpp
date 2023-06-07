#include "Camera.h"
#include "Util.h"
#include "WinAPI.h"

Camera::Camera() :
#pragma region ���������X�g
	eye_{ 0.0f, 0.0f, 0.0f },// ----> �J�����̍��W
	target_{ 0.0f, 0.0f, 0.0f },// -> �J�����̒����_
	up_{ 0.0f, 1.0f, 0.0f },// -----> �J�����̏�����x�N�g��

	matView_{},// -> �r���[�ϊ��s��
	matProjection_{}// -> �������e�s��
#pragma endregion
{
	// --�������e�s��̌v�Z-- //
	static XMMATRIX mat = XMMatrixPerspectiveFovLH(
		Util::Degree2Radian(45.0f),// -----------> �㉺��p45�x
		(float)WinAPI::GetWidth() / WinAPI::GetHeight(),// -> �A�X�y�N�g��i��ʉ���/��ʏc���j
		0.1f, 1000.0f);// ------------------------> �O�[�A���[

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matProjection_.m[i][j] = mat.r[i].m128_f32[j];
		}
	}
}

void Camera::Update() {
	// --�r���[�ϊ��s��-- //
	XMFLOAT3 eye = { eye_.x, eye_.y , eye_.z };
	XMFLOAT3 target = { target_.x, target_.y , target_.z };
	XMFLOAT3 up = { up_.x, up_.y , up_.z };

	XMMATRIX mat =
		XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matView_.m[i][j] = mat.r[i].m128_f32[j];
		}
	}
}