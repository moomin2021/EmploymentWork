#pragma region 自クラス読み込み(含める)
#include "WinAPI.h"// -> WinAPI
#include "DX12Cmd.h"// -> DirectX12
#include "Texture.h"// -> Texture
#include "Key.h"// -> キーボード入力
#include "Mouse.h"// -> マウスキーボード入力
#include "Object3D.h"// -> オブジェクト3Dクラス
#pragma endregion

#include "SceneManager.h"

// --Windowsアプリでのエントリーポイント(main関数)-- //
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// --WinAPIクラス-- //
	WinAPI* winAPI = WinAPI::GetInstance();// -> インスタンス取得
	winAPI->Initialize();// -> 初期化処理

	// --DirectX12クラス-- //
	DX12Cmd* dx12 = DX12Cmd::GetInstance();/// -> インスタンス取得
	dx12->Initialize(winAPI);// -> 初期化処理

	// --テクスチャクラス-- //
	Texture* texture = Texture::GetInstance();// -> インスタンス取得
	texture->Initialize(dx12->GetDevice());// -> 初期化処理

	// --キーボードクラス-- //
	Key* key = Key::GetInstance();
	key->Initialize(winAPI);

	// マウス入力
	Mouse* mouse = Mouse::GetInstance();
	mouse->Initialize(winAPI);

	// オブジェクト3Dを静的初期化
	Object3D::StaticInitialize(dx12->GetDevice(), dx12->GetCmdList());

	// --シーン管理クラス-- //
	SceneManager* sceneM = SceneManager::GetInstance();

	// --ゲームループ-- //
	while (true) {
		// --終了メッセージが来ていたらループ終了-- //
		if (winAPI->IsEndMessage() == true) break;

		// キーボード入力更新処理
		key->Update();

		// マウス入力更新処理
		mouse->Update();

		// シーン管理クラス更新処理
		sceneM->Update();

		// --描画前処理-- //
		dx12->PreDraw();

		// シーン管理クラス描画処理
		sceneM->Draw();

		// --描画後処理-- //
		dx12->PostDraw();
	}

	// --ウィンドウクラス登録解除-- //
	winAPI->WinClassUnregister();

	return 0;
}