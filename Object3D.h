#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl.h>
using namespace Microsoft::WRL;
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "fMath.h"
#include "LightGroup.h"

class Object3D {
public:// -----サブクラス----- //
	// 定数バッファ構造体(オブジェクト)
	struct ObjectBuff {
		Matrix4 viewProj;// -> ビュープロジェクション
		Matrix4 world;// ----> ワールド行列
		Float3 cameraPos;// -> カメラ座標(ワールド座標)
		float pad1;// -------> パディング
		Float4 color;// -----> 色(RGBA)
	};

private:// -----メンバ変数----- //
	Float3 position_;// ------------------> 座標
	Float3 rotation_;// ------------------> 回転角
	Float3 scale_;// ---------------------> スケール
	Float4 color_;// ---------------------> 色(RGBA)
	Matrix4 matWorld_;// -----------------> ワールド座標
	ComPtr<ID3D12Resource> constBuff_;// -> 定数バッファ
	Model* model_;// ---------------------> モデル
	bool dirty;// ------------------------> ダーティフラグ

private:// -----静的メンバ変数----- //
	static ID3D12Device* device_;// ---------------> デバイス
	static ID3D12GraphicsCommandList* cmdList_;// -> コマンドリスト
	static D3D12_HEAP_PROPERTIES heapProp_;// -----> ヒープ設定
	static D3D12_RESOURCE_DESC resdesc_;// --------> リソース設定
	static Camera* camera_;// ---------------------> カメラ
	static LightGroup* lightGroup_;// -------------> ライト

private:// -----メンバ関数----- //
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3D();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void GenerateConstBuffer();

	/// <summary>
	/// 定数バッファ更新
	/// </summary>
	void TransferConstBuffer();

public:// -----静的メンバ関数----- //
	/// <summary>
	/// [Object3D]インスタンス作成
	/// </summary>
	static Object3D* CreateObject3D(Model* model = nullptr);

	/// <summary>
	/// オブジェクト3D全体の初期化
	/// </summary>
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// カメラ設定
	/// </summary>
	static void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// ライト設定
	/// </summary>
	static void SetLightGroup(LightGroup* lightGroup) { lightGroup_ = lightGroup; }

public:// -----メンバ関数----- //
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 座標設定
	/// </summary>
	void SetPos(const Float3& position);

	/// <summary>
	/// 回転角設定[度数法]
	/// </summary>
	void SetRot(const Float3& rotation);

	/// <summary>
	/// 拡縮設定
	/// </summary>
	void SetScale(const Float3& scale);

	/// <summary>
	/// 色(RGBA)設定
	/// </summary>
	void SetColor(const Float4& color);

	/// <summary>
	/// モデル設定
	/// </summary>
	void SetModel(Model* model) { model_ = model; };

	/// <summary>
	/// 座標取得
	/// </summary>
	inline const Float3& GetPos() { return position_; }

	/// <summary>
	/// 回転角取得[度数法]
	/// </summary>
	inline const Float3& GetRot() { return rotation_; }

	/// <summary>
	/// 拡縮取得
	/// </summary>
	inline const Float3& GetScale() { return scale_; }
};