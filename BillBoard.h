#pragma once
// --Direct3D 12 用-- //
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// --数学関数-- //
#include <DirectXMath.h>
using namespace DirectX;

// --ComPtr用-- //
#include <wrl.h>
using namespace Microsoft::WRL;

// --シーケンスコンテナの一種-- //
#include <vector>

// --カメラクラス-- //
#include "Camera.h"

// --オブジェクト3Dクラス-- //
#include "Object3D.h"

enum BillBoardType {
	BILLBOARDX,
	BILLBOARDY,
	BILLBOARDZ,
	BILLBOARDALL,
};

class BillBoard {
	/// メンバ変数
public:
	// 座標、回転角、スケール
	XMFLOAT3 position_;
	float rotation_;
	XMFLOAT2 scale_;

	// 色
	XMFLOAT4 color_;

private:
	// --頂点データ-- //
	std::vector<Vertex3D> vertices_;// -> 頂点データ
	D3D12_VERTEX_BUFFER_VIEW vbView_;// -> 頂点バッファビュー
	ComPtr<ID3D12Resource> vertBuff_;// -> 頂点バッファ

	// --インデックスデータ-- //
	std::vector<uint16_t> indices_;// -> インデックスデータ
	D3D12_INDEX_BUFFER_VIEW ibView_;// -> インデックスバッファビュー
	ComPtr<ID3D12Resource> indexBuff_;// -> インデックスバッファ

	// --定数バッファ-- //
	ComPtr<ID3D12Resource> constBuff_;

	// --透視投影行列の計算-- //
	XMMATRIX matProjection_;

	/// メンバ関数
public:
	// --コンストラクタ-- //
	BillBoard();

	// 頂点バッファとインデックスバッファの作成
	void CreateBuffer();

	// --更新処理-- //
	void Update(Camera* camera, BillBoardType type);

	// --描画処理-- //
	void Draw(int textureHandle = 0);

	// --描画前処理-- //
	static void PreDraw();

private:
	// --四角形の頂点データとインデックスデータを作成-- //
	void CreateSquare();
};