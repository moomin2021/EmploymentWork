#pragma once
#include <DirectXMath.h>

class DirectionalLight {
private:// エイリアス
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:// サブクラス
	// 定数バッファ用データ構造
	struct ConstBufferData {
		XMVECTOR lightv;// -> ライトへ方向を表す
		XMFLOAT3 lightcolor;// -> ライトの色
		bool active;// -> 有効フラグ
	};

private:// メンバ変数
	// ライト光線方向
	XMVECTOR lightdir = { 1.0f, 0.0f, 0.0f, 0.0f };

	// ライト色
	XMFLOAT3 lightcolor = { 1.0f, 1.0f, 1.0f };

	// 有効フラグ
	bool active = false;

public:// メンバ関数
	inline void SetLightDir(const XMVECTOR& lightdir) { this->lightdir = DirectX::XMVector3Normalize(lightdir); }
	inline const XMVECTOR& GetLightDir() { return lightdir; }
	inline void SetLightColor(const XMFLOAT3& lightcolor) { this->lightcolor = lightcolor; };
	inline const XMFLOAT3& GetLightColor() { return lightcolor; }
	inline void SetActive(bool active) { this->active = active; }
	inline bool IsActive() { return active; }
};