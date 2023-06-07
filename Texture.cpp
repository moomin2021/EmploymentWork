#include "Texture.h"

#include "DX12Cmd.h"

// --SRVヒープの先頭ハンドルを取得-- //
D3D12_CPU_DESCRIPTOR_HANDLE Texture::srvHandle_ = {};

// --読み込む画像が何枚目か-- //
UINT Texture::imageCount_ = 0;

// --SRV用デスクリプタヒープ-- //
ComPtr<ID3D12DescriptorHeap> Texture::srvHeap_ = nullptr;

// テクスチャバッファ
std::map<const std::string, ComPtr<ID3D12Resource>> Texture::texBuff_ = {};

// テクスチャハンドル
std::map<const std::string, UINT> Texture::texHandle_ = {};

// --インスタンス読み込み-- //
Texture* Texture::GetInstance() {
	// --インスタンス生成-- //
	static Texture instance;

	// --インスタンスを返す-- //
	return &instance;
}

// --コンストラクタ-- //
Texture::Texture() 
#pragma region 初期化リスト
	//device_(nullptr)
#pragma endregion
{

}

// --デストラクタ-- //
Texture::~Texture() {}

// --初期化処理-- //
void Texture::Initialize(ID3D12Device* device) {
	// --DirectXクラスのデバイス取得-- //
	//this->device_ = device;

	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	/// --初期の真っ白なテクスチャを作成-- ///
#pragma region

	// --自作画像データ-- //
	const size_t textureWidth = 256;
	const size_t textureHeight = 256;
	const size_t imageDataCount = textureWidth * textureHeight;
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i].x = 1.0f;
		imageData[i].y = 1.0f;
		imageData[i].z = 1.0f;
		imageData[i].w = 1.0f;
	}

	// --ヒープ設定-- //
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	// --テクスチャバッファの生成-- //
	ComPtr<ID3D12Resource> texBuff;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	// --テクスチャバッファにデータ転送-- //
	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(XMFLOAT4) * textureWidth,
		sizeof(XMFLOAT4) * imageDataCount
	);
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

	// --デスクリプタヒープの設定-- //
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2056;

	// --設定をもとにSRV用デスクリプタヒープを生成-- //
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	// --SRVヒープの先頭ハンドルを取得-- //
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	// --シェーダリソースビュー設定-- //
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// 設定を保存
	texBuff_.emplace("white", texBuff);
	texHandle_.emplace("white", 0);

	// --ハンドルの指す①にシェーダーリソースビュー作成-- //
	device->CreateShaderResourceView(texBuff_["white"].Get(), &srvDesc, srvHandle_);
}

// --SRVヒープ参照-- //
ID3D12DescriptorHeap* Texture::GetSRVHeap() { return srvHeap_.Get(); }

int LoadTexture(const std::string fileName) {
	// 既に読み込んだ物だったら
	if (Texture::texBuff_.find(fileName) != Texture::texBuff_.end()) {
		return Texture::texHandle_[fileName];
	}

	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ユニコード文字列に変換する
	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, wfilepath, _countof(wfilepath));

	// --WICテクスチャのロード-- //
	result = LoadFromWICFile(
		wfilepath,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	ScratchImage mipChain{};

	// --ミップマップ生成-- //
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT,
		0,
		mipChain
	);

	// --読み込んだディフューズテクスチャをSRGBとして扱う-- //
	metadata.format = MakeSRGB(metadata.format);

	// --ヒープ設定-- //
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// --リソース設定-- //
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// --テクスチャバッファの生成-- //
	ComPtr<ID3D12Resource> texBuff = nullptr;
	result = DX12Cmd::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	// --全ミップマップについて-- //
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		// ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		// テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,// --------------> 全領域へコピー
			img->pixels,// ----------> 元データアドレス
			(UINT)img->rowPitch,// --> 1ラインサイズ
			(UINT)img->slicePitch// -> 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	// --CBV, SRV, UAVの1個分のサイズを取得-- //
	UINT descriptorSize = DX12Cmd::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// --ハンドルを1つ進める-- //
	Texture::srvHandle_.ptr += descriptorSize;

	// --画像カウンタインクリメント-- //
	Texture::imageCount_++;

	// 設定を保存
	Texture::texBuff_.emplace(fileName, texBuff);
	Texture::texHandle_.emplace(fileName, descriptorSize * Texture::imageCount_);

	// --シェーダリソースビュー設定-- //
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// --ハンドルの指す①にシェーダーリソースビュー作成-- //
	DX12Cmd::GetDevice()->CreateShaderResourceView(Texture::texBuff_[fileName].Get(), &srvDesc, Texture::srvHandle_);

	// --ハンドルを返す-- //
	return descriptorSize * Texture::imageCount_;
}
