#pragma once
// Direct3D 12 用
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// 数学関数
#include <DirectXMath.h>
using namespace DirectX;

//　ComPtr用
#include <wrl.h>
using namespace Microsoft::WRL;

// シーケンスコンテナの一種
#include <vector>

// 文字列クラス
#include <string>

// 頂点データ
struct Vertex3D {
	XMFLOAT3 pos;// ----> 座標
	XMFLOAT3 normal;// -> 法線
	XMFLOAT2 uv;// -----> UV座標
};

// 定数バッファ構造体(マテリアル)
struct MaterialBuff {
	XMFLOAT3 ambient;// -> アンビエント係数
	float pad1;// -> パディング
	XMFLOAT3 diffuse;// -> ディフェーズ係数
	float pad2;// -> パディング
	XMFLOAT3 specular;// -> スペキュラー係数
	float alpha;// -> アルファ
};

// マテリアル構造体
struct Material {
	std::string name;// -> マテリアル名
	XMFLOAT3 ambient;// -> アンビエント影響度
	XMFLOAT3 diffuse;// -> ディフューズ影響度
	XMFLOAT3 specular;// -> スペキュラー影響度
	float alpha;// -> アルファ
	std::string textureFilename;// -> テクスチャファイル名

	// コンストラクタ
	Material() {
		ambient = { 0.3f, 0.3f, 0.3f };
		diffuse = { 0.0f, 0.0f, 0.0f };
		specular = { 0.0f, 0.0f, 0.0f };
		alpha = 1.0f;
	}
};

class Model {
public:// メンバ変数
	std::vector<Vertex3D> vertexes_;// ----> 頂点データ
	D3D12_VERTEX_BUFFER_VIEW vbView_;// ---> 頂点バッファービュー
	ComPtr<ID3D12Resource> vertexBuff_;// -> 頂点バッファ

	std::vector<uint16_t> indexes_;// ----> インデックスデータ
	D3D12_INDEX_BUFFER_VIEW ibView_;// ---> インデックスバッファビュー
	ComPtr<ID3D12Resource> indexBuff_;// -> インデックスバッファ

	Material material_;// -------------------> マテリアルデータ
	ComPtr<ID3D12Resource> materialBuff_;// -> マテリアルバッファ

	// テクスチャハンドル
	int textureHandle_;

	static ID3D12GraphicsCommandList* cmdList_;// -> コマンドリスト

public:// メンバ関数
	// [Model]インスタンス作成
	static Model* CreateModel(std::string fileName);

	// 初期化処理
	static void Initialize(ID3D12GraphicsCommandList* cmdList);

	// 描画処理
	void Draw();

private:// メンバ関数
	// モデル読み込み
	void LoadModel(std::string name);

	// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& fileName);

	// 頂点バッファを作成
	void CreateVertexBuff();

	// インデックスバッファを作成
	void CreateIndexBuff();

	// マテリアルバッファ作成
	void CreateMaterialBuff();
};