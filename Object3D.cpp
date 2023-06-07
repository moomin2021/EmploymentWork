#include "Object3D.h"
#include "DX12Cmd.h"
#include "Texture.h"
#include "Util.h"

// 静的メンバ変数の実態
ID3D12Device* Object3D::device_ = nullptr;// ---------------> デバイス
ID3D12GraphicsCommandList* Object3D::cmdList_ = nullptr;// -> コマンドリスト
D3D12_HEAP_PROPERTIES Object3D::heapProp_ = {};// ----------> ヒープ設定
D3D12_RESOURCE_DESC Object3D::resdesc_ = {};// -------------> リソース設定
Camera* Object3D::camera_ = nullptr;// ---------------------> カメラ
LightGroup* Object3D::lightGroup_ = nullptr;// -------------> ライト

Object3D::Object3D() :
#pragma region 初期化リスト
	position_{0.0f, 0.0f, 0.0f},// ----> 座標
	rotation_{0.0f, 0.0f, 0.0f},// ----> 回転角
	scale_{1.0f, 1.0f, 1.0f},// -------> スケール
	color_{1.0f, 1.0f, 1.0f, 1.0f},// -> 色(RGBA)
	matWorld_{},// --------------------> ワールド座標
	constBuff_(nullptr),// ------------> 定数バッファ
	model_(nullptr),// ----------------> モデル
	dirty(true)// ---------------------> ダーティフラグ
#pragma endregion
{
	// 定数バッファ生成
	GenerateConstBuffer();
}

void Object3D::GenerateConstBuffer() {
	// 関数実行の成否を判別用の変数
	HRESULT result;

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProp_,
		D3D12_HEAP_FLAG_NONE,
		&resdesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void Object3D::TransferConstBuffer() {
	// ダーティーフラグが[ON]なら
	if (dirty) {
		// ワールド座標計算
		Matrix4 matScale, matRot, matTrans;
		matScale = Matrix4Scale(scale_);
		matRot = Matrix4Identity();
		matRot *= Matrix4RotateZ(Util::Degree2Radian(rotation_.z));
		matRot *= Matrix4RotateX(Util::Degree2Radian(rotation_.x));
		matRot *= Matrix4RotateY(Util::Degree2Radian(rotation_.y));
		matTrans = Matrix4Translate(position_);

		// ワールド行列の合成
		matWorld_ = Matrix4Identity();// -> 変形のリセット
		matWorld_ *= matScale;// ---------> ワールド行列にスケーリングを反映
		matWorld_ *= matRot;// -----------> ワールド行列に回転を反映
		matWorld_ *= matTrans;// ---------> ワールド行列に平行移動を反映

		// ダーティーフラグ[OFF]にする
		dirty = false;
	}

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// マッピング
	ObjectBuff* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	constMap->viewProj = camera_->GetMatView() * camera_->GetMatProjection();
	constMap->world = matWorld_;
	constMap->cameraPos = camera_->eye_;
	constMap->color = color_;

	// マッピング終了処理
	constBuff_->Unmap(0, nullptr);
}

Object3D* Object3D::CreateObject3D(Model* model)
{
	// インスタンス生成
	Object3D* object = new Object3D();

	// モデル設定
	object->SetModel(model);

	// 生成したインスタンスを返す
	return object;
}

void Object3D::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
	device_ = device;// -> デバイス
	cmdList_ = cmdList;// -> コマンドリスト

	// モデル初期化
	Model::Initialize(cmdList);

	// 定数バッファのヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 定数バッファのリソース設定
	resdesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc_.Width = (sizeof(ObjectBuff) + 0xff) & ~0xff;
	resdesc_.Height = 1;
	resdesc_.DepthOrArraySize = 1;
	resdesc_.MipLevels = 1;
	resdesc_.SampleDesc.Count = 1;
	resdesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void Object3D::PreDraw() {
	// パイプラインステートの設定
	cmdList_->SetPipelineState(DX12Cmd::GetObject3DPipeline().pipelineState.Get());
	
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(DX12Cmd::GetObject3DPipeline().rootSignature.Get());
	
	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetSRVHeap() };
	cmdList_->SetDescriptorHeaps(1, ppHeaps);
}

void Object3D::Draw()
{
	// オブジェクトの更新処理
	TransferConstBuffer();

	// --定数バッファビュー（CBV）の設定コマンド-- //
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// ライトの描画
	lightGroup_->Draw();

	// モデルの情報を元に描画
	model_->Draw();
}

void Object3D::SetPos(const Float3& position)
{
	position_ = position;// -> 座標設定
	dirty = true;// -> ダーティーフラグを[ON]にする
}

void Object3D::SetRot(const Float3& rotation)
{
	rotation_ = rotation;// -> 回転角設定
	dirty = true;// -> ダーティーフラグを[ON]にする
}

void Object3D::SetScale(const Float3& scale)
{
	scale_ = scale;// -> 拡縮設定
	dirty = true;// -> ダーティーフラグを[ON]にする
}

void Object3D::SetColor(const Float4& color) {
	color_ = color;// -> 色(RGBA)設定
}