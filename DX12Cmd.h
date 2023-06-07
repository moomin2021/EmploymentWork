#pragma once
// --WinAPI-- //
#include "WinAPI.h"

// --Direct3D 12 用-- //
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// --DXGI用-- //
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

// --式が真であることを判定する機能の追加-- //
#include <cassert>

// --ComPtr用-- //
#include <wrl.h>
using namespace Microsoft::WRL;

// --シーケンスコンテナの一種-- //
#include <vector>

// --文字列-- //
#include <string>

// --時間を扱う為の標準ライブラリ-- //
#include <chrono>

// --パイプライン-- //
#include "Pipeline.h"

class DX12Cmd
{
#pragma region メンバ変数
public:

private:
	static ComPtr<ID3D12Device> device_;// -> デバイス
	ComPtr<IDXGIFactory7> dxgiFactory;// -> DXGIファクトリー
	ComPtr<IDXGISwapChain4> swapChain;// -> スワップチェーン
	ComPtr<ID3D12CommandAllocator> cmdAllocator;// -> コマンドアロケータ
	static ComPtr<ID3D12GraphicsCommandList> commandList;// -> コマンドリスト
	ComPtr<ID3D12CommandQueue> commandQueue;// -> コマンドキュー
	ComPtr<ID3D12DescriptorHeap> rtvHeap;// -> レンダーターゲットビュー
	std::vector<ComPtr<ID3D12Resource>> backBuffers;// -> バックバッファ
	D3D12_RESOURCE_BARRIER barrierDesc;// -> リソースバリア
	ComPtr<ID3D12Fence> fence;// -> フェンス
	UINT64 fenceVal;// -> フェンス値
	ComPtr<ID3D12DescriptorHeap> dsvHeap;// -> 深度ステンシルビュー
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12Resource> depthBuff;// -> リソース設定

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

	// --スプライト用のパイプライン-- //
	static PipelineSet spritePipeline_;

	// --オブジェクト3D用のパイプライン-- //
	static PipelineSet object3DPipeline_;

	// --ビルボード用のパイプライン-- //
	//static PipelineSet billBoardPipeline_;

#pragma endregion

#pragma region メンバ関数
public:
	// --インスタンス読み込み-- //
	static DX12Cmd* GetInstance();

	// --初期化処理-- //
	void Initialize(WinAPI * win);

	// FPS固定初期化処理
	void InitializeFixFPS();

	// FPS固定更新処理
	void UpdateFixFPS();

	// --描画前処理-- //
	void PreDraw();

	// --描画後処理-- //
	void PostDraw();

#pragma region ゲッター
	// --デバイスを取得-- //
	static ID3D12Device* GetDevice() { return device_.Get(); }

	// --コマンドリストを取得-- //
	static ID3D12GraphicsCommandList* GetCmdList() { return commandList.Get(); }

	// --スプライト用のパイプラインを取得-- //
	static PipelineSet GetSpritePipeline() { return spritePipeline_; }

	// --オブジェクト3D用のパイプラインを取得-- //
	static PipelineSet GetObject3DPipeline() { return object3DPipeline_; }

	// --ビルボード用のパイプラインを取得
	//static PipelineSet GetBillBoardPipeline() { return billBoardPipeline_; }
#pragma endregion

private:
	// --コンストラクタ-- //
	DX12Cmd();

	// --デストラクタ-- //
	~DX12Cmd();
#pragma endregion

	// --禁止-- //
	DX12Cmd(const DX12Cmd&) = delete;// --------------> コピーコンストラクタ禁止
	DX12Cmd& operator = (const DX12Cmd&) = delete;// -> コピー代入演算子禁止
};

