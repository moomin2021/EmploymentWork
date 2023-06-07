#pragma once
// DirectXInput
#include <dinput.h>

// ComPtr用
#include <wrl.h>
using namespace Microsoft::WRL;

// WinAPI
#include "WinAPI.h"

// Vector2
#include "Vector2.h"

enum MouseButton {
	M_LEFT,
	M_RIGHT,
	M_CENTER
};

class Mouse {
	// メンバ変数
public:

private:
	// デバイス
	ComPtr<IDirectInputDevice8> device_;

	// 現在のマウスの入力状態
	DIMOUSESTATE nowMouse_;

	// 前フレームのマウスの入力状態
	DIMOUSESTATE oldMouse_;

	// マウスカーソル座標
	POINT p_;

	// WinAPI
	WinAPI* win_;

	// メンバ関数
public:
	// インスタンス読み込み
	static Mouse* GetInstance();

	// 初期化処理
	void Initialize(WinAPI* win);

	// 更新処理
	void Update();

	/// <summary>
	/// 指定されたマウスのボタンが押されていたら、1を返し、そうでなかった場合0を返す
	/// </summary>
	/// <param name="button"> 判定したいマウスボタン </param>
	/// <returns></returns>
	bool PushMouseButton(MouseButton button) { return nowMouse_.rgbButtons[button] == (0x80); }

	/// <summary>
	/// 指定されたマウスのボタンが押された瞬間だったら、1を返し、そうでなかった場合0を返す
	/// </summary>
	/// <param name="button"> 判定したいマウスボタン </param>
	/// <returns></returns>
	bool TriggerMouseButton(MouseButton button) { return nowMouse_.rgbButtons[button] == (0x80) && !(oldMouse_.rgbButtons[button] == (0x80)); }

	// --マウスの位置-- //
	Vector2 MousePos() { return Vector2(p_.x, p_.y); }

	// --マウスの移動量-- //
	Vector2 GetMouseVelosity() { return Vector2((float)nowMouse_.lX, (float)nowMouse_.lY); }

private:
	// コンストラクタ
	Mouse();

	// デストラクタ
	~Mouse();

};