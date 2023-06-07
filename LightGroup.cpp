#include "LightGroup.h"
#include <d3dx12.h>

using namespace DirectX;

// 静的メンバ変数の実態
ID3D12Device* LightGroup::device = nullptr;

void LightGroup::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!LightGroup::device);

	// nullptrチェック
	assert(device);

	LightGroup::device = device;
}

LightGroup* LightGroup::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();

	// 初期化
	instance->Initialize();

	return instance;
}

void LightGroup::Initialize()
{
	// 標準のライトの設定
	DefaultLightSetting();

	// ヒープ設定
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// 定数バッファの生成
	HRESULT result;
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	TransferConstBuffer();
}

void LightGroup::TransferConstBuffer()
{
	HRESULT result;

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {

		// 環境光
		constMap->ambientColor = ambientColor;

		// 平行光源
		for (size_t i = 0; i < DirLightNum; i++) {
			// ライトが有効なら設定を転送
			if (dirLights[i].IsActive()) {
				constMap->dirLights[i].active = true;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightcolor = dirLights[i].GetLightColor();
			}

			// ライトが無効なら転送しない
			else {
				constMap->dirLights[i].active = false;
			}
		}

		// 点光源
		for (size_t i = 0; i < PointLightNum; i++) {
			// ライトが有効なら設定を転送
			if (pointLights[i].IsActive()) {
				constMap->pointLights[i].active = true;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights[i].GetLightAtten();
			}

			// ライトが無効ならライト色を0に
			else {
				constMap->pointLights[i].active = false;
			}
		}

		// スポットライト
		for (size_t i = 0; i < SpotLightNum; i++) {
			// ライトが有効なら設定を転送
			if (spotLights[i].IsActive()) {
				constMap->spotLights[i].active = true;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightpos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightcolor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos = spotLights[i].GetLightFactorAngleCos();
			}

			// ライトが無効ならライト色を0に
			else {
				constMap->spotLights[i].active = false;
			}
		}

		// 丸影
		for (int i = 0; i < CircleShadowNum; i++) {
			// 有効なら設定を転送
			if (circleShadows[i].IsActive()) {
				constMap->circleShadows[i].active = true;
				constMap->circleShadows[i].dir = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows[i].GetFactorAngleCos();
			}

			// 無効なら色を0に
			else {
				constMap->circleShadows[i].active = false;
			}
		}

		constBuff->Unmap(0, nullptr);
	}
}

void LightGroup::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetActive(active);
	dirty = true;
}

void LightGroup::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetActive(active);
	dirty = true;
}

void LightGroup::SetPointLightPos(int index, const XMFLOAT3& lightpos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightPos(lightpos);
	dirty = true;
}

void LightGroup::SetPointLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetPointLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetActive(active);
	dirty = true;
}

void LightGroup::SetSpotLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetSpotLightPos(int index, const XMFLOAT3& lightpos)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightPos(lightpos);
	dirty = true;
}

void LightGroup::SetSpotLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetSpotLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightFactorAngle(lightFactorAngle);
	dirty = true;
}

void LightGroup::SetCircleShadowActive(int index, bool active)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetActive(active);
	dirty = true;
}

void LightGroup::SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetCasterPos(casterPos);
	dirty = true;
}

void LightGroup::SetCircleShadowDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDir(lightdir);
	dirty = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDistanceCasterLight(distanceCasterLight);
	dirty = true;
}

void LightGroup::SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetCircleShadowFactorAngle(int index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetFactorAngle(lightFactorAngle);
	dirty = true;
}

void LightGroup::DefaultLightSetting()
{
	dirLights[0].SetActive(true);
	dirLights[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[0].SetLightDir({0.0f, -1.0f, 0.0f, 0.0f});

	dirLights[1].SetActive(true);
	dirLights[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[1].SetLightDir({ 0.5f, 0.1f, 0.2f, 0.0f });

	dirLights[2].SetActive(true);
	dirLights[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[2].SetLightDir({ -0.5f, 0.1f, -0.2f, 0.0f });
}

void LightGroup::Update()
{
	// 値の更新があった時だけ定数バッファに転送する
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void LightGroup::Draw()
{
	// 定数バッファビューをセット
	DX12Cmd::GetCmdList()->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());
}
