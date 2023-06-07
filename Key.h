#pragma once
// --DirectXInput-- //
#define DIREXTINPUT_VERSION 0x0800// DirectInputのバージョン指定
#include <dinput.h>

// --ComPtr用-- //
#include <wrl.h>

#include "WinAPI.h"

class Key {
	// namespaceの省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// --メンバ関数-- //
public:
	// インスタンス読み込み
	static Key* GetInstance();

	// 初期化処理
	void Initialize(WinAPI * win);

	// 更新処理
	void Update();

	/// <summary>
	/// 指定されたキーが押されていたら、1を返し、そうでなかったら0を返す
	/// </summary>
	/// <param name="key"> 判定したいキー </param>
	/// <returns></returns>
	bool PushKey(UINT key) { return keys_[key]; }

	/// <summary>
	/// 指定されたキーが押された瞬間だったら、1を返し、そうでなかったら0を返す
	/// </summary>
	/// <param name="key"> 判定したいキー </param>
	/// <returns></returns>
	bool TriggerKey(UINT8 key) { return keys_[key] && !oldKeys_[key]; }

private:
	// デストラクタ
	~Key();

	// --メンバ変数-- //
public:
	
private:
	// 現在のキーボードの情報
	UINT8 keys_[256];

	// 前フレームのキーボードの情報
	UINT8 oldKeys_[256];

	// デバイス
	ComPtr<IDirectInputDevice8> device_;

};