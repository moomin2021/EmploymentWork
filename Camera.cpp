#include "Camera.h"
#include "Util.h"
#include "WinAPI.h"

Camera::Camera() :
#pragma region 初期化リスト
	eye_{ 0.0f, 0.0f, 0.0f },// ----> カメラの座標
	target_{ 0.0f, 0.0f, 0.0f },// -> カメラの注視点
	up_{ 0.0f, 1.0f, 0.0f },// -----> カメラの上方向ベクトル

	matView_{},// -> ビュー変換行列
	matProjection_{}// -> 透視投影行列
#pragma endregion
{
	// --透視投影行列の計算-- //
	static XMMATRIX mat = XMMatrixPerspectiveFovLH(
		Util::Degree2Radian(45.0f),// -----------> 上下画角45度
		(float)WinAPI::GetWidth() / WinAPI::GetHeight(),// -> アスペクト比（画面横幅/画面縦幅）
		0.1f, 1000.0f);// ------------------------> 前端、奥端

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matProjection_.m[i][j] = mat.r[i].m128_f32[j];
		}
	}
}

void Camera::Update() {
	// --ビュー変換行列-- //
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