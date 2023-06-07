#include "WinAPI.h"
#pragma comment(lib, "winmm.lib")

// --ウィンドウプロシージャプロトタイプ宣言-- //
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// --インスタンス読み込み-- //
WinAPI* WinAPI::GetInstance() {
	// --インスタンス生成-- //
	static WinAPI win;

	// --インスタンスを返す-- //
	return &win;
}

// --ウィンドウサイズ-- //
const int WinAPI::winWidth_ = 1280;
const int WinAPI::winHeight_ = 720;

// --コンストラクタ-- //
WinAPI::WinAPI() :
#pragma region 初期化リスト
	winClass_{},// -> ウィンドウクラス
	winRect_{},// -> ウィンドウサイズ
	hwnd_{},// -> ウィンドウハンドル(オブジェクト)
	msg_{}// -> メッセージ
#pragma endregion
{

}

// --デストラクタ-- //
WinAPI::~WinAPI() {
	
}

// --初期化処理-- //
void WinAPI::Initialize() {
	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	// --ウィンドウクラスの設定-- //
	winClass_.cbSize = sizeof(WNDCLASSEX);// -> WNDCLASSEX構造体のサイズ
	winClass_.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定(アドレスを代入)
	winClass_.lpszClassName = L"DirectXGame"; // ウィンドウクラス名
	winClass_.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	winClass_.hCursor = LoadCursor(NULL, IDC_ARROW); // マウスカーソル指定

	// --ウィンドウクラスをOSに登録する-- //
	RegisterClassEx(&winClass_);

	// --ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }-- //
	winRect_ = { 0, 0, winWidth_, winHeight_ };

	// --自動でサイズを補正する-- //
	AdjustWindowRect(&winRect_, WS_OVERLAPPEDWINDOW, false);

	// --ウィンドウオブジェクトの生成-- //
	hwnd_ = CreateWindow(
		winClass_.lpszClassName,// ------> クラス名
		L"DirectXGame",// -------> タイトルバーの文字
		WS_OVERLAPPEDWINDOW,// --> 標準的なウィンドウスタイル
		CW_USEDEFAULT,// --------> 表示X座標(OSに任せる)
		CW_USEDEFAULT,// --------> 表示Y座標(OSに任せる)
		winRect_.right - winRect_.left,// -> ウィンドウ横幅
		winRect_.bottom - winRect_.top,// -> ウィンドウ縦幅
		nullptr,// --------------> 親ウィンドウハンドル
		nullptr,// --------------> メニューハンドル
		winClass_.hInstance,// ----------> 呼び出しアプリケーションハンドル
		nullptr);// -------------> オプション

	// --ウィンドウを表示状態にする-- //
	ShowWindow(hwnd_, SW_SHOW);
}

// --終了メッセージがきているか参照-- //
bool WinAPI::IsEndMessage() {
	// --メッセージがあるか-- //
	if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg_); // キー入力メッセージの処理
		DispatchMessage(&msg_); // プロシージャにメッセージを送る
	}

	// --×ボタンで終了メッセージが来たらゲームループを抜ける-- //
	if (msg_.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

// --ウィンドウクラス登録解除-- //
void WinAPI::WinClassUnregister() { UnregisterClass(winClass_.lpszClassName, winClass_.hInstance); }

// --ウィンドウプロシージャ(システムメッセージを処理するための関数)-- //
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}