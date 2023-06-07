#include "Pipeline.h"

// --DirectX3Dクラス-- //
#include "DX12Cmd.h"

//// --Direct3D 12のヘルパー構造と関数-- //
//#include<d3dx12.h>

// --スプライト用パイプライン生成-- //
PipelineSet CreateSpritePipeline() {
	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	// --この関数で最終的に返す変数-- //
	PipelineSet pipelineSet;

	/// --シェーダーの読み込みとコンパイル-- ///
#pragma region

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// --頂点シェーダの読み込みとコンパイル-- //
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		vsBlob.GetAddressOf(), errorBlob.GetAddressOf());

	// --エラーなら-- //
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// --ピクセルシェーダの読み込みとコンパイル-- //
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		psBlob.GetAddressOf(), errorBlob.GetAddressOf());

	// --エラーなら-- //
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion
	/// --END-- ///

		/// --頂点レイアウト-- ///
#pragma region

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",// -> セマンティック名
			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32B32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
			0,// -> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
			0// -> 一度に描画するインスタンス数（0でよい）
		},

		{
			"TEXCOORD",// -> セマンティック名
			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
			0,// -> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
			0// -> 一度に描画するインスタンス数（0でよい）
		},
	};

#pragma endregion
	/// --END-- ///

	/// --グラフィックスパイプライン-- ///
#pragma region

	// --グラフィックスパイプライン設定-- //
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// --シェーダーの設定-- //
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// --サンプルマスクの設定-- //
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// --ラスタライザの設定-- //
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面を科リング
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// --デプスステンシルステート-- //
	//pipelineDesc.DepthStencilState.DepthEnable = true;// -> 深度テストを行う
	//pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// -> 書き込み許可
	//pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;// -> 小さければ合格
	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット

	// --レンダーターゲットのブレンド設定-- //
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画

	// --アルファ値共通設定-- //
	blenddesc.BlendEnable = true;// ----------------> ブレンドを有効する
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// -> 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ---> ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// -> デストの値を0%使う

	////加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;			//使わない
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//// --ブレンドステートの設定-- //
	//pipelineDesc.BlendState.RenderTarget[0] = blenddesc;
	//pipelineDesc.BlendState.AlphaToCoverageEnable = true;

	//// --深度バッファのフォーマット-- //
	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// --頂点レイアウトの設定-- //
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// --図形の形状設定-- //
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// --その他の設定-- //
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// --デスクリプタレンジの設定-- //
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;// -> 1度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;// -> テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// --ルートパラメータの設定-- //
	D3D12_ROOT_PARAMETER rootParams[2] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数のバッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;// ------------------> 定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える

	// -- テクスチャレジスタ0番-- //
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// -> 種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;// -------> デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;// --------------------> デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -------------> 全てのシェーダから見える

	// --テクスチャサンプラーの設定-- //
	// ※テクスチャがオブジェクトに張り付くときの拡大縮小の補間方法などを指定するもの //
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;// -> ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// ------------------> 全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;// --------------------------------> ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;// ---------------------------------------------> ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// -> ピクセルシェーダーからのみ使用可能

	// --ルートシグネチャの設定-- //
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;// -------------> ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// --ルートシグネチャのシリアライズ-- //
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = DX12Cmd::GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootSignature));
	assert(SUCCEEDED(result));

	// --パイプラインにルートシグネチャをセット-- //
	pipelineDesc.pRootSignature = pipelineSet.rootSignature.Get();

	// --パイプランステートの生成-- //
	result = DX12Cmd::GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineSet.pipelineState));
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

	// --設定したパイプラインを返す-- //
	return pipelineSet;
}
PipelineSet CreateObject3DPipeline()
{
	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	// --この関数で最終的に返す変数-- //
	PipelineSet pipelineSet;

	/// --シェーダーの読み込みとコンパイル-- ///
#pragma region

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// --頂点シェーダの読み込みとコンパイル-- //
	result = D3DCompileFromFile(
		L"Resources/Shaders/Object3DVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		vsBlob.GetAddressOf(), errorBlob.GetAddressOf());

	// --エラーなら-- //
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// --ピクセルシェーダの読み込みとコンパイル-- //
	result = D3DCompileFromFile(
		L"Resources/Shaders/Object3DPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		psBlob.GetAddressOf(), errorBlob.GetAddressOf());

	// --エラーなら-- //
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion
	/// --END-- ///

		/// --頂点レイアウト-- ///
#pragma region

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",// -> セマンティック名
			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32B32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
			0,// -> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
			0// -> 一度に描画するインスタンス数（0でよい）
		},

		{
			"NORMAL",// -> セマンティック名
			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32B32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
			0,// -> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
			0// -> 一度に描画するインスタンス数（0でよい）
		},

		{
			"TEXCOORD",// -> セマンティック名
			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
			0,// -> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
			0// -> 一度に描画するインスタンス数（0でよい）
		},
	};

#pragma endregion
	/// --END-- ///

	/// --グラフィックスパイプライン-- ///
#pragma region

	// --グラフィックスパイプライン設定-- //
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// --シェーダーの設定-- //
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// --サンプルマスクの設定-- //
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// --ラスタライザの設定-- //
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面を科リング
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// --デプスステンシルステート-- //
	pipelineDesc.DepthStencilState.DepthEnable = true;// -> 深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// -> 書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;// -> 小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット

	// --レンダーターゲットのブレンド設定-- //
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画

	// --アルファ値共通設定-- //
	blenddesc.BlendEnable = true;// ----------------> ブレンドを有効する
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// -> 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ---> ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// -> デストの値を0%使う

	////加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;			//使わない
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//// --ブレンドステートの設定-- //
	//pipelineDesc.BlendState.RenderTarget[0] = blenddesc;
	//pipelineDesc.BlendState.AlphaToCoverageEnable = true;

	//// --深度バッファのフォーマット-- //
	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// --頂点レイアウトの設定-- //
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// --図形の形状設定-- //
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// --その他の設定-- //
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// --デスクリプタレンジの設定-- //
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;// -> 1度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;// -> テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// --ルートパラメータの設定-- //
	D3D12_ROOT_PARAMETER rootParams[4] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数のバッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;// ------------------> 定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える

	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数のバッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1;// ------------------> 定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える

	// -- テクスチャレジスタ0番-- //
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// -> 種類
	rootParams[2].DescriptorTable.pDescriptorRanges = &descriptorRange;// -------> デスクリプタレンジ
	rootParams[2].DescriptorTable.NumDescriptorRanges = 1;// --------------------> デスクリプタレンジ数
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -------------> 全てのシェーダから見える

	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数のバッファビュー
	rootParams[3].Descriptor.ShaderRegister = 2;// ------------------> 定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える

	// --テクスチャサンプラーの設定-- //
	// ※テクスチャがオブジェクトに張り付くときの拡大縮小の補間方法などを指定するもの //
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;// -> ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// ------------------> 全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;// --------------------------------> ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;// ---------------------------------------------> ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// -> ピクセルシェーダーからのみ使用可能

	// --ルートシグネチャの設定-- //
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;// -------------> ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// --ルートシグネチャのシリアライズ-- //
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = DX12Cmd::GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootSignature));
	assert(SUCCEEDED(result));

	// --パイプラインにルートシグネチャをセット-- //
	pipelineDesc.pRootSignature = pipelineSet.rootSignature.Get();

	// --パイプランステートの生成-- //
	result = DX12Cmd::GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineSet.pipelineState));
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

	// --設定したパイプラインを返す-- //
	return pipelineSet;
}
//PipelineSet CreateBillBoardPipeline()
//{
//	// --関数が成功したかどうかを判別する用変数-- //
//	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
//	HRESULT result;
//
//	// --この関数で最終的に返す変数-- //
//	PipelineSet pipelineSet;
//
//	/// --シェーダーの読み込みとコンパイル-- ///
//#pragma region
//
//	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
//	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
//	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト
//
//	// --頂点シェーダの読み込みとコンパイル-- //
//	result = D3DCompileFromFile(
//		L"Object3DVS.hlsl", // シェーダファイル名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
//		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
//		0,
//		vsBlob.GetAddressOf(), errorBlob.GetAddressOf());
//
//	// --エラーなら-- //
//	if (FAILED(result))
//	{
//		// errorBlobからエラー内容をstring型にコピー
//		std::string error;
//		error.resize(errorBlob->GetBufferSize());
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		// エラー内容を出力ウィンドウに表示
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//	// --ピクセルシェーダの読み込みとコンパイル-- //
//	result = D3DCompileFromFile(
//		L"Object3DPS.hlsl", // シェーダファイル名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
//		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
//		0,
//		psBlob.GetAddressOf(), errorBlob.GetAddressOf());
//
//	// --エラーなら-- //
//	if (FAILED(result))
//	{
//		// errorBlobからエラー内容をstring型にコピー
//		std::string error;
//		error.resize(errorBlob->GetBufferSize());
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		// エラー内容を出力ウィンドウに表示
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//#pragma endregion
//	/// --END-- ///
//
//		/// --頂点レイアウト-- ///
//#pragma region
//
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		{
//			"POSITION",// -> セマンティック名
//			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
//			DXGI_FORMAT_R32G32B32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
//			0,// -> 入力スロットインデックス（0でよい）
//			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
//			0// -> 一度に描画するインスタンス数（0でよい）
//		},
//
//		{
//			"NORMAL",// -> セマンティック名
//			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
//			DXGI_FORMAT_R32G32B32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
//			0,// -> 入力スロットインデックス（0でよい）
//			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
//			0// -> 一度に描画するインスタンス数（0でよい）
//		},
//
//		{
//			"TEXCOORD",// -> セマンティック名
//			0,// -> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
//			DXGI_FORMAT_R32G32_FLOAT,// -> 要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
//			0,// -> 入力スロットインデックス（0でよい）
//			D3D12_APPEND_ALIGNED_ELEMENT,// -> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,// -> 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
//			0// -> 一度に描画するインスタンス数（0でよい）
//		},
//	};
//
//#pragma endregion
//	/// --END-- ///
//
//	/// --グラフィックスパイプライン-- ///
//#pragma region
//
//	// --グラフィックスパイプライン設定-- //
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
//
//	// --シェーダーの設定-- //
//	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
//	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
//	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
//	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
//
//	// --サンプルマスクの設定-- //
//	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
//
//	// --ラスタライザの設定-- //
//	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面を科リング
//	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
//	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
//	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に
//
//	// --デプスステンシルステート-- //
//	pipelineDesc.DepthStencilState.DepthEnable = true;// -> 深度テストを行う
//	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;// -> 書き込み許可
//	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;// -> 小さければ合格
//	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット
//
//	// --レンダーターゲットのブレンド設定-- //
//	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
//	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
//
//	// --アルファ値共通設定-- //
//	blenddesc.BlendEnable = true;// ----------------> ブレンドを有効する
//	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// -> 加算
//	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ---> ソースの値を100%使う
//	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// -> デストの値を0%使う
//
//	////加算合成
//	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
//	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
//	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
//	////減算合成
//	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
//	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
//	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う
//	////色反転
//	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
//	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
//	//blenddesc.DestBlend = D3D12_BLEND_ZERO;			//使わない
//	//半透明合成
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
//	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
//	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
//
//	//// --ブレンドステートの設定-- //
//	//pipelineDesc.BlendState.RenderTarget[0] = blenddesc;
//	//pipelineDesc.BlendState.AlphaToCoverageEnable = true;
//
//	//// --深度バッファのフォーマット-- //
//	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	// --頂点レイアウトの設定-- //
//	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
//	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
//
//	// --図形の形状設定-- //
//	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//
//	// --その他の設定-- //
//	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
//	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
//	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
//
//	// --デスクリプタレンジの設定-- //
//	D3D12_DESCRIPTOR_RANGE descriptorRange{};
//	descriptorRange.NumDescriptors = 1;// -> 1度の描画に使うテクスチャが1枚なので1
//	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRange.BaseShaderRegister = 0;// -> テクスチャレジスタ0番
//	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	// --ルートパラメータの設定-- //
//	D3D12_ROOT_PARAMETER rootParams[2] = {};
//	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数のバッファビュー
//	rootParams[0].Descriptor.ShaderRegister = 0;// ------------------> 定数バッファ番号
//	rootParams[0].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
//	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える
//
//	// -- テクスチャレジスタ0番-- //
//	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// -> 種類
//	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;// -------> デスクリプタレンジ
//	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;// --------------------> デスクリプタレンジ数
//	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -------------> 全てのシェーダから見える
//
//	// --テクスチャサンプラーの設定-- //
//	// ※テクスチャがオブジェクトに張り付くときの拡大縮小の補間方法などを指定するもの //
//	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
//	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 横繰り返し（タイリング）
//	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 縦繰り返し（タイリング）
//	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// -> 奥行繰り返し（タイリング）
//	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;// -> ボーダーの時は黒
//	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// ------------------> 全てリニア補間
//	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;// --------------------------------> ミップマップ最大値
//	samplerDesc.MinLOD = 0.0f;// ---------------------------------------------> ミップマップ最小値
//	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
//	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// -> ピクセルシェーダーからのみ使用可能
//
//	// --ルートシグネチャの設定-- //
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	rootSignatureDesc.pParameters = rootParams;// -------------> ルートパラメータの先頭アドレス
//	rootSignatureDesc.NumParameters = _countof(rootParams);
//	rootSignatureDesc.pStaticSamplers = &samplerDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	// --ルートシグネチャのシリアライズ-- //
//	ComPtr<ID3DBlob> rootSigBlob = nullptr;
//	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&rootSigBlob, &errorBlob);
//	assert(SUCCEEDED(result));
//	result = DX12Cmd::GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&pipelineSet.rootSignature));
//	assert(SUCCEEDED(result));
//
//	// --パイプラインにルートシグネチャをセット-- //
//	pipelineDesc.pRootSignature = pipelineSet.rootSignature.Get();
//
//	// --パイプランステートの生成-- //
//	result = DX12Cmd::GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineSet.pipelineState));
//	assert(SUCCEEDED(result));
//
//#pragma endregion
//	/// --END-- ///
//
//	// --設定したパイプラインを返す-- //
//	return pipelineSet;
//}