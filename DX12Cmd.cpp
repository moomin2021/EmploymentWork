#include "DX12Cmd.h"
#include <thread>

// --インスタンス読み込み-- //
DX12Cmd* DX12Cmd::GetInstance() {
	// --インスタンス生成-- //
	static DX12Cmd dx12;

	// --インスタンスを返す-- //
	return &dx12;
}

// --デバイス-- //
ComPtr<ID3D12Device> DX12Cmd::device_ = nullptr;

// --コマンドリスト-- //
ComPtr<ID3D12GraphicsCommandList> DX12Cmd::commandList = nullptr;

// --スプライト用のパイプライン-- //
PipelineSet DX12Cmd::spritePipeline_ = { nullptr, nullptr };
PipelineSet DX12Cmd::object3DPipeline_ = { nullptr, nullptr };
//PipelineSet DX12Cmd::billBoardPipeline_ = { nullptr, nullptr };

// --コンストラクタ-- //
DX12Cmd::DX12Cmd() :
#pragma region 初期化リスト
	dxgiFactory(nullptr),// -> DXGIファクトリー
	cmdAllocator(nullptr),// -> コマンドアロケータ
	commandQueue(nullptr),// -> コマンドキュー
	swapChain(nullptr),// -> スワップチェーン
	rtvHeap(nullptr),// -> レンダーターゲットビュー
	backBuffers{},// -> バックバッファ
	barrierDesc{},// -> リソースバリア
	fence(nullptr),// -> フェンス
	fenceVal(0)// -> フェンス値
#pragma endregion
{

}

// --デストラクタ-- //
DX12Cmd::~DX12Cmd() {

}

void DX12Cmd::Initialize(WinAPI* win) {
	// FPS固定初期化処理
	InitializeFixFPS();

	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	/// ※Visual Studioの「出力」ウィンドウで追加のエラーメッセージが表示できるように ///
#pragma region デバックレイヤーの有効か

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma endregion
	/// --END-- ///

	/// ※PCにあるグラフィックボードを、仮想的なデバイスを含めて全部リストアップする ///
#pragma region アダプタの列挙

	// --DXGIファクトリーの生成-- //
	// DXGI = グラフィックスインフラストラクチャ
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	// --アダプターの列挙用-- //
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	// --ここに特定の名前を持つアダプターオブジェクトが入る-- //
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	// --パフォーマンスが高いものから順に、全てのアダプターを列挙する-- //
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

#pragma endregion
	/// --END-- ///

	/// ※検出されたグラフィックスデバイスの中で性能の低いもの除外して、専用デバイスを採用する ///
#pragma region アダプタの選別

	// --妥当なアダプタを選別する-- //
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;

		// アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			// デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}

#pragma endregion
	/// --END-- ///

	/// ※採用したグラフィックスデバイスを操作するためにD3D12Deviceオブジェクトを生成 ///
	/// ※これは普通１ゲームに1つしか作らない ///
#pragma region デバイスの生成

	// --対応レベルの配列-- //
	D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	};

	// --グラフィックスデバイスを操作する為のオブジェクト生成-- //
	// ※これは普通、１ゲームに１つしか作らない
	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++)
	{
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK)
		{
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

#pragma endregion
	/// --END-- ///

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);	//やばいエラーの時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);		//エラーの時止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);	//警告の時止まる
	}

	//抑制するエラー
	D3D12_MESSAGE_ID denyIds[] = {
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};

	//抑制される表示レベル
	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(severities);
	filter.DenyList.pSeverityList = severities;
	//指定したエラーの表示を抑制する
	infoQueue->PushStorageFilter(&filter);
#endif

	/// ※GPUに、まとめて命令を送るためのコマンドリストを生成する //
#pragma region コマンドリスト

	// --コマンドアロケータを生成-- //
	// ※コマンドリストはコマンドアロケータから生成するので、先にコマンドアロケータを作る //
	// ※コマンドリストに格納する命令の為のメモリを管理するオブジェクト //
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,// -> コマンドアロケータの種類
		IID_PPV_ARGS(&cmdAllocator));// -> 各インターフェイス固有のGUID
	assert(SUCCEEDED(result));// -> ID3D12CommandAllocatorインターフェイスのポインタを格納する変数アドレス

	// --コマンドリストを生成-- //
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

	/// ※コマンドリストをGPUに順に実行させていく為の仕組み ///
#pragma region コマンドキュー

	// --コマンドキューの設定-- //
	// ※{}をつけることで構造体の中身を0でクリアしている。
	// ※値0が標準値になるようにMicrosoftが決めているので、この場合コマンドキューの設定を標準にしている //
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	// --標準設定でコマンドキューを生成-- //
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///

	/// ※スワップチェーンは、ダブルバッファリングやトリプルバッファリングを簡単に実装するための仕組み ///
	/// ※表示中の画面（フロントバッファ）・描画中の画面（バックバッファ）
#pragma region スワップチェーン

	// --スワップチェーンの設定-- //
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	// --画面の幅を指定する
	swapChainDesc.Width = win->GetWidth();;

	// --画面の高さを指定する
	swapChainDesc.Height = win->GetHeight();

	// --色情報の書式（表示形式）
	//※DXGI_FORMAT_R8G8B8A8_UNORMはアルファを含むチャンネルあたり8ビットをサポート
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// --ピクセルあたりのマルチサンプルの数を指定する
	swapChainDesc.SampleDesc.Count = 1;

	// --リソースの使用方法を指定
	// ※DXGI_USAGE_BACK_BUFFERはリソースをバックバッファとして使用する
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;

	// --スワップチェーン内のバッファーの数を指定する
	swapChainDesc.BufferCount = 2;

	// --画面をスワップした後のリソースの処理方法を指定
	// ※DXGI_SWAP_EFFECT_FLIP_DISCARDはスワップした後バックバッファーの内容を破棄する設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// --スワップチェーン動作のオプションを指定
	// ※DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCHはウィンドウとフルスクリーンの切り替え時に>>
	// >>解像度がウィンドウサイズに一致するように変更する
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// --IDXGISwapChain1のComPtrを用意-- //
	ComPtr<IDXGISwapChain1> swapChain1;

	// --スワップチェーンの生成-- //
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(), win->GetHWND(), &swapChainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&swapChain1);
	assert(SUCCEEDED(result));

	// --生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する-- //
	swapChain1.As(&swapChain);

#pragma endregion
	/// --END-- ///

	/// ※バックバッファを描画キャンパスとして扱う為のオブジェクト //
	/// ※ダブルバッファリングではバッファが２つあるので２つ作る //
#pragma region レンダーターゲットビュー

	// ※レンダーターゲットビューはデスクリプタヒープに生成するので、先にデスクリプタヒープを作る //
	// --デスクリプタヒープの設定-- //
	//D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // 裏表の2つ

	// --デスクリプタヒープの生成-- //
	device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

	// ※スワップチェーン内に生成されたバックバッファのアドレスを入れておく
	// --バックバッファ-- //
	backBuffers.resize(swapChainDesc.BufferCount);

	// --スワップチェーンの全てのバッファについて処理する-- //
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		// --スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));

		// --デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

		// --裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

		// --レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

		// --シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// --レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}

#pragma endregion
	/// --END-- ///

	/// ※CPUとGPUで同期をとるためのDirectXの仕組み ///
#pragma region フェンスの生成

	result = device_->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

#pragma endregion
	/// --END-- ///

	/// --深度バッファ-- ///
#pragma region 深度バッファ
	// --リソース設定-- //
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = win->GetWidth();// ---> レンダーターゲットに合わせる
	depthResourceDesc.Height = win->GetHeight();// -> レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// -> デプスステンシル

	// --深度値用ヒーププロパティ-- //
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// --深度値のクリア設定-- //
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;// -> 深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット

	// --リソース生成-- //
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,// -> 深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	// --深度ビュー用デスクリプタヒープ作成-- //
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;// -> 深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;// -> デプスステンシルビュー
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	// --深度ビュー作成-- //
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;// -> 深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);
#pragma endregion

	// --スプライト用のパイプラインを生成-- //
	spritePipeline_ = CreateSpritePipeline();

	// --オブジェクト3D用のパイプラインを生成-- //
	object3DPipeline_ = CreateObject3DPipeline();

	// --ビルボード用のパイプラインを生成-- //
	//billBoardPipeline_ = CreateBillBoardPipeline();
}

void DX12Cmd::InitializeFixFPS()
{
	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DX12Cmd::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

// --描画前処理-- //
void DX12Cmd::PreDraw() {
	/// --1.リソースバリアで書き込み可能に変更-- ///
#pragma region
	// --バックバッファの番号を取得(2つなので0番か1番)-- //
	UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

	barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // バックバッファを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	commandList->ResourceBarrier(1, &barrierDesc);

#pragma endregion
	/// --END-- ///

	/// --2.描画先の変更-- ///
#pragma region

		// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

	//// --深度ステンシルビュー用デスクリプタヒープのハンドルを取得-- //
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

#pragma endregion
	/// ※これ以降の描画コマンドでは、ここで指定した描画キャンパスに絵を描いていくことになる ///
	/// --END-- ///

	/// --3.画面クリア R G B A-- ///
	/// ※バックバッファには前回に描いた絵がそのまま残っているので、一旦指定色で塗りつぶす ///
#pragma region

	FLOAT clearColor[] = { 0.1f, 0.25, 0.5f, 0.0f }; // 青っぽい色
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

#pragma endregion
	/// --END-- ///

	/// --ビューポート設定-- ///
#pragma region

		// --ビューポート設定コマンド-- //
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinAPI::GetWidth();
	viewport.Height = WinAPI::GetHeight();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// --ビューポート設定コマンドを、コマンドリストに積む-- //
	commandList->RSSetViewports(1, &viewport);

#pragma endregion
	/// --END-- ///

	/// --シザー矩形-- ///
#pragma region

		// --シザー矩形-- //
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = scissorRect.left + WinAPI::GetWidth(); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + WinAPI::GetHeight(); // 切り抜き座標下

	// --シザー矩形設定コマンドを、コマンドリストに積む-- //
	commandList->RSSetScissorRects(1, &scissorRect);

#pragma endregion
	/// --END-- ///
}

// --描画後処理-- //
void DX12Cmd::PostDraw() {
	// --関数が成功したかどうかを判別する用変数-- //
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成 //
	HRESULT result;

	/// --5.リソースバリアを戻す-- ///
#pragma region

	// --バックバッファを書き込み可能状態から画面表示状態に変更する
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
	commandList->ResourceBarrier(1, &barrierDesc);

	// --ここまでため込んだコマンドを実行し描画する処理-- //
	{
		// --命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));

		// --コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);

		// --画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));
	}
	// --END-- //

	// --コマンドの実行完了を待つ-- //
	commandQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// FPS固定
	UpdateFixFPS();

	// --キューをクリア-- //
	// ※次の使用に備えてコマンドアロケータとコマンドリストをリセットしておく //
	result = cmdAllocator->Reset();
	assert(SUCCEEDED(result));

	// --再びコマンドリストを貯める準備-- //
	result = commandList->Reset(cmdAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));

#pragma endregion
	/// --END-- ///
}