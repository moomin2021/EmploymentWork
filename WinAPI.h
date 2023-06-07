#pragma once
// --WindowsAPI-- //
#include <Windows.h>

class WinAPI {
#pragma region メンバ変数
public:

private:
	// --ウィンドウサイズ-- //
	static const int winWidth_;// --> 横幅
	static const int winHeight_;// -> 縦幅

	WNDCLASSEX winClass_;// -> ウィンドウクラス
	RECT winRect_;// -> ウィンドウサイズ
	HWND hwnd_;// -> ウィンドウハンドル(オブジェクト)
	MSG msg_;// -> メッセージ

#pragma endregion

#pragma region メンバ関数
public:
	// --インスタンス読み込み-- //
	static WinAPI* GetInstance();

	// --初期化処理-- //
	void Initialize();

	// --終了メッセージがきているか参照-- //
	bool IsEndMessage();

	// --ウィンドウクラス登録解除-- //
	void WinClassUnregister();

#pragma region ゲッター
	// --ウィンドゥクラスを取得-- //
	WNDCLASSEX GetWNDCLASSEX() { return winClass_; }

	// --ウィンドウハンドル(オブジェクト)を取得-- //
	HWND GetHWND() { return hwnd_; }

	// --ウィンドウの横サイズを取得-- //
	static int GetWidth() { return winWidth_; }

	// --ウィンドウの縦サイズを取得-- //
	static int GetHeight() { return winHeight_; }
#pragma endregion

private:
	// --コンストラクタ-- //
	WinAPI();

	// --デストラクタ-- //
	~WinAPI();
#pragma endregion

	// --禁止-- //
	WinAPI(const WinAPI&) = delete;// --------------> コピーコンストラクタ禁止
	WinAPI& operator = (const WinAPI&) = delete;// -> コピー代入演算子禁止
};