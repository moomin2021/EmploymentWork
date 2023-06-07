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

// --頂点データ-- //
struct Vertices2D {
	// --XYZ座標-- //
	XMFLOAT3 pos;

	// --uv座標-- //
	XMFLOAT2 uv;
};

// --定数バッファ用データ構造体（行列と色）-- //
struct ConstBufferData {
	// --色（RBGA）-- //
	XMFLOAT4 color;

	// --行列-- //
	XMMATRIX mat;
};

class Sprite {
	/// --メンバ変数-- ///
public:
	// --Z軸回りの回転角-- //
	float rotation;

	// --座標-- //
	XMFLOAT2 position;

	// --ワールド行列-- //
	XMMATRIX matWorld;

	// --射影変換行列-- //
	XMMATRIX matProjection;

	// --色（RBGA）-- //
	XMFLOAT4 color;

	// --スケール-- //
	XMFLOAT2 scale;

private:
	// --頂点データ-- //
	Vertices2D vertices[4];

	// --インデックスデータ-- //
	uint16_t indices[6];

	// --頂点バッファビュー-- //
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// --頂点バッファ-- //
	ComPtr<ID3D12Resource> vertBuff;

	// --頂点バッファマッピング処理用-- //
	Vertices2D* vertMap;

	// --インデックスバッファ-- //
	ComPtr<ID3D12Resource> indexBuff;

	// --インデックスバッファビュー-- //
	D3D12_INDEX_BUFFER_VIEW ibView;

	// --定数バッファ-- //
	ComPtr<ID3D12Resource> constBuff;

	// --定数バッファマッピング処理用-- //
	ConstBufferData* constMap;

	/// --メンバ変数END-- ///
	/// --------------- ///
	/// --メンバ関数-- ///
public:
	// --コンストラクタ-- //
	Sprite();

	// --更新処理-- //
	void Update();

	// --描画処理-- //
	void Draw(int textureHandle = 0);

	// --描画前処理-- //
	static void PreDraw();

private:

	/// --メンバ関数END-- ///
};