#include "Sprite.h"

// --WinAPIクラス-- //
#include "WinAPI.h"

// --DirextX12-- //
#include "DX12Cmd.h"

// --テクスチャクラス-- //
#include "Texture.h"

// --便利系関数-- //
#include "Util.h"

// --コンストラクタ-- //
Sprite::Sprite() : vbView{}, ibView{}, constBuff(nullptr), constMap(nullptr), vertBuff(nullptr), vertMap(nullptr),
position{0.0f, 0.0f}, color {1.0f, 1.0f, 1.0f, 1.0f}, scale{ 1.0f, 1.0f }
{
	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	// --頂点データ-- //
	vertices[0] = { {   0.0f, 100.0f * scale.y, 0.0f }, {0.0f, 1.0f} };// -> 左下
	vertices[1] = { {   0.0f,   0.0f, 0.0f }, {0.0f, 0.0f} };// -> 左上
	vertices[2] = { { 100.0f * scale.x, 100.0f * scale.y, 0.0f }, {1.0f, 1.0f} };// -> 右下
	vertices[3] = { { 100.0f * scale.x,   0.0f, 0.0f }, {1.0f, 0.0f} };// -> 右上

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	// --頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	/// --頂点バッファの確保-- ///
#pragma region

	// --頂点バッファの設定-- //
	D3D12_HEAP_PROPERTIES heapVSProp{}; // ヒープ設定
	heapVSProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// --リソース設定-- //
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// --頂点バッファの生成-- //
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&heapVSProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

		/// --頂点バッファへのデータ転送-- ///
#pragma region

	// --GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得-- //
	//Vertices* vertMap = nullptr;

	// --Map処理でメインメモリとGPUのメモリを紐づける-- //
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// --全頂点に対して-- //
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}

#pragma endregion
	/// --END-- ///

	/// --頂点バッファビューの作成-- ///
#pragma region

	// --GPU仮想アドレス-- //
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();

	// --頂点バッファのサイズ-- //
	vbView.SizeInBytes = sizeVB;

	// --頂点1つ分のデータサイズ-- //
	vbView.StrideInBytes = sizeof(vertices[0]);

#pragma endregion
	/// --END-- ///

	// --インデックスデータ全体サイズ-- //
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// --リリース設定-- //
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // -> インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// --インデックスバッファの生成-- //
	//ID3D12Resource* indexBuff = nullptr;
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&heapVSProp,// -> ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,// -> リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	// --インデックスバッファをマッピング-- //
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	// --全インデックスに対して-- //
	for (int i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}

	// --マッピング解除-- //
	indexBuff->Unmap(0, nullptr);

	// --インデックスバッファビューの作成-- //
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	/// --定数バッファ-- ///
#pragma region

	// --定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// --定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// --定数バッファの生成
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	// --定数バッファのマッピング
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	// --スプライトの色を変える-- //
	constMap->color = color;

	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)WinAPI::GetWidth(), (float)WinAPI::GetHeight(), 0.0f, 0.0f, 1.0f);

	// --行列初期化-- //
	constMap->mat = XMMatrixIdentity();

#pragma endregion

}

// --更新処理-- //
void Sprite::Update() {
	// --ワールド行列更新-- //
	matWorld = XMMatrixIdentity();

	// --Z軸回転-- //
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));

	// --平行移動-- //
	matWorld *= XMMatrixTranslation(position.x, position.y, 0.0f);

	// --定数バッファの転送-- //

	// --行列計算-- //
	constMap->mat = matWorld * matProjection;

	// --色適用-- //
	constMap->color = color;

	// --頂点バッファ転送-- //
	// --頂点データ-- //
	vertices[0] = { {   0.0f, 100.0f * scale.y, 0.0f }, {0.0f, 1.0f} };// -> 左下
	vertices[1] = { {   0.0f,   0.0f, 0.0f }, {0.0f, 0.0f} };// -> 左上
	vertices[2] = { { 100.0f * scale.x, 100.0f * scale.y, 0.0f }, {1.0f, 1.0f} };// -> 右下
	vertices[3] = { { 100.0f * scale.x,   0.0f, 0.0f }, {1.0f, 0.0f} };// -> 右上

	// --全頂点に対して-- //
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}
}

// --描画処理-- //
void Sprite::Draw(int textureHandle) {
	// --SRVヒープのハンドルを取得-- //
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = Texture::GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();

	// --ハンドルを指定された分まで進める-- //
	srvGpuHandle.ptr += textureHandle;

	// --指定されたSRVをルートパラメータ1番に設定-- //
	DX12Cmd::GetCmdList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// --頂点バッファビューの設定コマンド-- //
	DX12Cmd::GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

	// --定数バッファビュー（CBV）の設定コマンド-- //
	DX12Cmd::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// --インデックスバッファビューの設定コマンド-- //
	DX12Cmd::GetCmdList()->IASetIndexBuffer(&ibView);

	//// --描画コマンド-- //
	DX12Cmd::GetCmdList()->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
}

// --描画前処理-- //
void Sprite::PreDraw()
{
	// --コマンドリスト取得-- //
	//ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetCmdList();

	// パイプラインステートの設定
	DX12Cmd::GetCmdList()->SetPipelineState(DX12Cmd::GetSpritePipeline().pipelineState.Get());
	// ルートシグネチャの設定
	DX12Cmd::GetCmdList()->SetGraphicsRootSignature(DX12Cmd::GetSpritePipeline().rootSignature.Get());
	// プリミティブ形状を設定
	DX12Cmd::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// --デスクリプタヒープの配列をセットするコマンド-- //
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetSRVHeap() };
	DX12Cmd::GetCmdList()->SetDescriptorHeaps(1, ppHeaps);
}
