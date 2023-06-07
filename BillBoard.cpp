#include "BillBoard.h"

// --DirextX12-- //
#include "DX12Cmd.h"

// --テクスチャクラス-- //
#include "Texture.h"

// --便利系関数-- //
#include "Util.h"

// --モデル読み込みのため-- //
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

BillBoard::BillBoard() :
#pragma region 初期化リスト
	// 座標、回転角、スケール
	position_{ 0.0f, 0.0f, 0.0f },
	rotation_(0.0f),
	scale_{ 1.0f, 1.0f },

	// 色
	color_{ 1.0f, 1.0f, 1.0f, 1.0f },

	// --透視投影行列の計算-- //
	matProjection_{},

	// --頂点データ-- //
	vertices_{},// -> 頂点データ
	vbView_{},// -> 頂点バッファビュー
	vertBuff_(nullptr),// -> 頂点バッファ

	// --インデックスデータ-- //
	indices_{},// -> インデックスデータ
	ibView_{},// -> インデックスバッファビュー
	indexBuff_(nullptr),// -> インデックスバッファ

	// --定数バッファ-- //
	constBuff_(nullptr)
#pragma endregion
{
#pragma region 定数バッファ初期化処理
	// 関数が成功したかどうかを判別する用変数
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成
//	HRESULT result;
//
//	// 定数バッファのヒープ設定
//	D3D12_HEAP_PROPERTIES heapProp{};
//	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//	// 定数バッファのリソース設定
//	D3D12_RESOURCE_DESC resdesc{};
//	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resdesc.Width = (sizeof(ObjectBuff) + 0xff) & ~0xff;
//	resdesc.Height = 1;
//	resdesc.DepthOrArraySize = 1;
//	resdesc.MipLevels = 1;
//	resdesc.SampleDesc.Count = 1;
//	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	// 定数バッファの生成
//	result = DX12Cmd::GetDevice()->CreateCommittedResource(
//		&heapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resdesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&constBuff_));
//	assert(SUCCEEDED(result));
//
//	// 定数バッファのマッピング
//	ObjectBuff* constMap;
//	result = constBuff_->Map(0, nullptr, (void**)&constMap);
//	assert(SUCCEEDED(result));
//
//	// マッピング終了
//	constBuff_->Unmap(0, nullptr);
//#pragma endregion
//
//	// --透視投影行列の計算-- //
//	matProjection_ = XMMatrixPerspectiveFovLH(
//		Util::Degree2Radian(45.0f),// -----------> 上下画角45度
//		(float)WinAPI::GetWidth() / WinAPI::GetHeight(),// -> アスペクト比（画面横幅/画面縦幅）
//		0.1f, 1000.0f// ------------------------> 前端、奥端
//	);
//
//	// 四角形の頂点データとインデックスデータを作成
//	CreateSquare();
}

void BillBoard::Update(Camera* camera, BillBoardType type)
{
	// 視点座標
	//XMVECTOR eyePos = XMLoadFloat3(&camera->eye_);

	//// 注視点座標
	//XMVECTOR targetPos = XMLoadFloat3(&camera->target_);

	//// 上方向
	//XMVECTOR upVec = XMLoadFloat3(&camera->up_);

	//// カメラZ軸
	//XMVECTOR cameraAxisZ = XMVectorSubtract(targetPos, eyePos);

	//// 0ベクトルだと向きが定まらないので除外 //
	//assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	//assert(!XMVector3IsInfinite(cameraAxisZ));
	//assert(!XMVector3Equal(upVec, XMVectorZero()));
	//assert(!XMVector3IsInfinite(upVec));

	//// ベクトルを正規化 //
	//cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//// カメラのX軸(右方向) //
	//XMVECTOR cameraAxisX;

	//// X軸は上方向→Z軸の外積で求まる //
	//cameraAxisX = XMVector3Cross(upVec, cameraAxisZ);

	//// ベクトルを正規化 //
	//cameraAxisX = XMVector3Normalize(cameraAxisX);

	//// カメラのY軸(上方向) //
	//XMVECTOR cameraAxisY;

	//// Y軸はZ軸→X軸の外積で求まる //
	//cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	//// カメラ回転行列 //
	//XMMATRIX matCameraRot;

	//// カメラ座標系→ワールド座標系の変換行列 //
	//matCameraRot.r[0] = cameraAxisX;
	//matCameraRot.r[1] = cameraAxisY;
	//matCameraRot.r[2] = cameraAxisZ;
	//matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//XMMATRIX matBillboard;

	//if (type == BILLBOARDX) {

	//}

	//else if (type == BILLBOARDY) {
	//	// カメラX軸、Y軸、Z軸 //
	//	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//	// X軸は共通 //
	//	ybillCameraAxisX = cameraAxisX;

	//	// Y軸はワールド座標系のY軸 //
	//	ybillCameraAxisY = XMVector3Normalize(upVec);

	//	// Z軸はX軸→Y軸の外積で求まる //
	//	ybillCameraAxisZ = XMVector3Cross(cameraAxisX, cameraAxisY);

	//	// Y軸回りのビルボード行列 //
	//	matBillboard.r[0] = ybillCameraAxisX;
	//	matBillboard.r[1] = ybillCameraAxisY;
	//	matBillboard.r[2] = ybillCameraAxisZ;
	//	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
	//}

	//else if (type == BILLBOARDZ) {

	//}

	//else if (type == BILLBOARDALL) {
	//	// 全方向ビルボード行列の計算 //
	//	matBillboard.r[0] = cameraAxisX;
	//	matBillboard.r[1] = cameraAxisY;
	//	matBillboard.r[2] = cameraAxisZ;
	//	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
	//}

	// 転置により逆行列(逆回転)を計算 //
	//matView = XMMatrixTranspose(matCameraRot);

	// 関数が成功したかどうかを判別する用変数
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成
	HRESULT result;

	// --スケール、回転、平行移動行列の計算-- //
	XMMATRIX matScale, matRot, matTrans;
	matScale = XMMatrixScaling(scale_.x, scale_.y, 1.0f);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(Util::Degree2Radian(rotation_));
	matTrans = XMMatrixTranslation(position_.x, position_.y, position_.z);

	// --ワールド行列の合成-- //
	XMMATRIX matWorld = XMMatrixIdentity();// -> 変形のリセット
	//matWorld *= matBillboard;
	matWorld *= matScale;// -> ワールド行列にスケーリングを反映
	matWorld *= matRot;// -> ワールド行列に回転を反映
	matWorld *= matTrans;// -> ワールド行列に平行移動を反映

	// --マッピング-- //
	//ObjectBuff* constMap = nullptr;
	//result = constBuff_->Map(0, nullptr, (void**)&constMap);
	//assert(SUCCEEDED(result));

	// --定数バッファへデータ転送-- //
	//constMap->mat = matWorld * camera->GetMatView() * matProjection_;// -> 行列

	// --繋がりを解除-- //
	//constBuff_->Unmap(0, nullptr);
}

void BillBoard::Draw(int textureHandle)
{
	// --SRVヒープのハンドルを取得-- //
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = Texture::GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();

	// --ハンドルを指定された分まで進める-- //
	srvGpuHandle.ptr += textureHandle;

	// --指定されたSRVをルートパラメータ1番に設定-- //
	DX12Cmd::GetCmdList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// --頂点バッファビューの設定コマンド-- //
	DX12Cmd::GetCmdList()->IASetVertexBuffers(0, 1, &vbView_);

	// --インデックスバッファビューの設定コマンド-- //
	DX12Cmd::GetCmdList()->IASetIndexBuffer(&ibView_);

	// --定数バッファビュー（CBV）の設定コマンド-- //
	DX12Cmd::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	//// --描画コマンド-- //
	DX12Cmd::GetCmdList()->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void BillBoard::PreDraw()
{
	//// パイプラインステートの設定
	//DX12Cmd::GetCmdList()->SetPipelineState(DX12Cmd::GetBillBoardPipeline().pipelineState.Get());
	//// ルートシグネチャの設定
	//DX12Cmd::GetCmdList()->SetGraphicsRootSignature(DX12Cmd::GetBillBoardPipeline().rootSignature.Get());
	//// プリミティブ形状を設定
	//DX12Cmd::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// --デスクリプタヒープの配列をセットするコマンド-- //
	//ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetSRVHeap() };
	//DX12Cmd::GetCmdList()->SetDescriptorHeaps(1, ppHeaps);
}

void BillBoard::CreateSquare()
{
	// 関数が成功したかどうかを判別する用変数
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成
	HRESULT result;

#pragma region 頂点データ作成
	// 立方体頂点データ
	Vertex3D vertices[] = {
		// 前面
		{{ -0.5f, -0.5f, -0.5f}, {}, {0.0f, 1.0f}},// -> 左下 0
		{{ -0.5f,  0.5f, -0.5f}, {}, {0.0f, 0.0f}},// -> 左上 1
		{{  0.5f, -0.5f, -0.5f}, {}, {1.0f, 1.0f}},// -> 右下 2
		{{  0.5f,  0.5f, -0.5f}, {}, {1.0f, 0.0f}},// -> 右上 3
	};

	// 頂点データをコピー
	for (size_t i = 0; i < _countof(vertices); i++) {
		vertices_.push_back(vertices[i]);
	}
#pragma endregion

#pragma region インデックスデータ作成
	// 立方体インデックスデータ
	uint16_t indices[] = {
		// 前面
		0, 1, 2,// -> 三角形1つ目
		2, 1, 3,// -> 三角形2つ目
	};

	// インデックスデータをコピー
	for (size_t i = 0; i < _countof(indices); i++) {
		indices_.push_back(indices[i]);
	}
#pragma endregion

#pragma region 法線計算
	for (size_t i = 0; i < indices_.size() / 3; i++) {
		// --三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indices_[i * 3 + 0];
		unsigned short index1 = indices_[i * 3 + 1];
		unsigned short index2 = indices_[i * 3 + 2];

		// --三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices_[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices_[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices_[index2].pos);

		// --p0->p1ベクトル、p0->p2ベクトルを計算（ベクトルの減算）
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		// --外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		// --正規化
		normal = XMVector3Normalize(normal);

		// --求めた法線を頂点データに代入
		XMStoreFloat3(&vertices_[index0].normal, normal);
		XMStoreFloat3(&vertices_[index1].normal, normal);
		XMStoreFloat3(&vertices_[index2].normal, normal);
	}
#pragma endregion

	// 頂点バッファとインデックスバッファの作成
	CreateBuffer();
}

void BillBoard::CreateBuffer()
{
	// 関数が成功したかどうかを判別する用変数
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成
	HRESULT result;

#pragma region 頂点バッファ作成
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * vertices_.size());

	// --頂点バッファの設定-- //
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

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
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// --頂点バッファビューの作成-- //
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();// -> GPU仮想アドレス
	vbView_.SizeInBytes = sizeVB;// -> 頂点バッファのサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);// -> 頂点1つ分のデータサイズ

	// --Map処理でメインメモリとGPUのメモリを紐づける-- //
	Vertex3D* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// --全頂点に対して-- //
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertMap[i] = vertices_[i]; // 座標をコピー
	}

	// --繋がりを解除-- //
	vertBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region インデックスバッファ作成
	// --インデックスデータ全体のサイズ-- //
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

	// --リソース設定-- //
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// --インデックスバッファの生成-- //
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&heapProp,// -> ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,// -> リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_)
	);

	// --インデックスバッファビュー作成-- //
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	// --インデックスバッファをマッピング-- //
	uint16_t* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));

	// --全インデックスに対して-- //
	for (size_t i = 0; i < indices_.size(); i++)
	{
		indexMap[i] = indices_[i];
	}

	// --マッピング解除-- //
	indexBuff_->Unmap(0, nullptr);
#pragma endregion
}
