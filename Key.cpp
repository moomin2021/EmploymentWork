#include "Key.h"
#include <cassert>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Key* Key::GetInstance() {
	// インスタンス生成
	static Key key;

	// --インスタンスを返す-- //
	return &key;
}

Key::~Key() {
	// キーボードデバイスの制御制御
	device_->Unacquire();

	// キーボードデバイスの解放
	device_->Release();
}

void Key::Initialize(WinAPI * win) {
	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

#pragma region DirectInputの初期化

	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		win->GetWNDCLASSEX().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

#pragma endregion
	
#pragma region デバイスの生成

	// デバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &device_, NULL);
	assert(SUCCEEDED(result));

	// キーボード入力データの形式のセット
	result = device_->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = device_->SetCooperativeLevel(
		win->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

	// デバイス制御開始
	device_->Acquire();

#pragma endregion
}

void Key::Update() {
	// 前フレームのキーの状態を保存
	for (size_t i = 0; i < 256; i++) oldKeys_[i] = keys_[i];

	// 全キーの入力状態を取得する
	device_->GetDeviceState(sizeof(keys_), keys_);
}