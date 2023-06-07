#include "Mouse.h"
#include <cassert>
#define DIREXTINPUT_VERSION 0x0800// DirectInputのバージョン指定
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Mouse* Mouse::GetInstance()
{
	// 関数内static変数として宣言
	static Mouse instance;
	return &instance;
}

Mouse::Mouse() :
#pragma region 初期化リスト
	device_(nullptr),// -> デバイス
	nowMouse_{},// -> 現在のマウスの状態
	oldMouse_{}// -> 前フレームのマウスの状態
#pragma endregion
{

}

Mouse::~Mouse()
{
	// デバイス制御
	device_->Unacquire();

	// デバイスの解放
	device_->Release();
}

void Mouse::Initialize(WinAPI* win)
{
	win_ = win;

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// DirectInputデバイス生成
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		win->GetWNDCLASSEX().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

	// マウスデバイスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &device_, NULL);
	assert(SUCCEEDED(result));

	// マウス入力データの形式のセット
	result = device_->SetDataFormat(&c_dfDIMouse);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = device_->SetCooperativeLevel(
		win->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

	// マウスデバイス制御開始
	device_->Acquire();

	// DirectInputデバイスの解放
	directInput->Release();
}

void Mouse::Update()
{
	// --前フレームのマウスの入力状態を保存-- //
	oldMouse_ = nowMouse_;

	// --マウスの入力状態を取得-- //
	device_->GetDeviceState(sizeof(DIMOUSESTATE), &nowMouse_);

	// --マウスの座標を取得-- //
	GetCursorPos(&p_);
	ScreenToClient(FindWindowW(win_->GetWNDCLASSEX().lpszClassName, nullptr), &p_);
}
