#pragma once
// --Direct3D 12 用-- //
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// --シェーダの読み込みとコンパイル用-- //
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// --ComPtr用-- //
#include <wrl.h>
using namespace Microsoft::WRL;

// --文字列-- //
#include <string>

// --式が真であることを判定する機能の追加-- //
#include <cassert>

// --パイプラインセット-- //
struct PipelineSet {
	// --パイプラインステートオブジェクト-- //
	ComPtr<ID3D12PipelineState> pipelineState;

	// --ルートシグネチャ-- //
	ComPtr<ID3D12RootSignature> rootSignature;
};

// --スプライト用パイプライン生成-- //
PipelineSet CreateSpritePipeline();

// --オブジェクト3Dのパイプライン生成-- //
PipelineSet CreateObject3DPipeline();

// --ビルボード用のパイプライン生成-- //
PipelineSet CreateBillBoardPipeline();