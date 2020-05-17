#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//初期化に必要な変数たち
HRESULT result;
ID3D12Device* dev = nullptr;
IDXGIFactory6* dxgiFactory = nullptr;
IDXGISwapChain4* swapchain = nullptr;
ID3D12CommandAllocator* cmdAllocator = nullptr;
ID3D12GraphicsCommandList* cmdList = nullptr;
ID3D12CommandQueue* cmdQueue = nullptr;
ID3D12DescriptorHeap* rtvHeaps = nullptr;

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された
		PostQuitMessage(0);//OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//標準の処理を行う

}

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//DirectX初期化処理ここから

		//ウィンドウサイズ
	const int window_width = 1280;//横幅
	const int window_height = 720;//縦幅

	WNDCLASSEX w{};//ウィンドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	//ウィンドウクラスにOSを登録
	RegisterClassEx(&w);
	//ウィンドウサイズ(x座標,y座標,横幅,縦幅)
	RECT wrc = { 0,0,window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//自動でサイズ補正

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,//クラス名
		L"DirectXGame",                      //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,                 //標準的なウィンドウスタイル
		CW_USEDEFAULT,                       //表示x座標(OSに任せる)
		CW_USEDEFAULT,                       //表示y座標(OSに任せる)
		wrc.right - wrc.left,                //ウィンドウ横幅
		wrc.bottom - wrc.top,                //ウィンドウ縦幅
		nullptr,                             //親ウィンドウハンドル
		nullptr,                             //メニューハンドル
		w.hInstance,                         //呼び出しアプリケーションハンドル
		nullptr);                            //オプション

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

//アダプタの列挙
//

//DXGIファクトリーの生成
result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
//アダプターの列挙用
std::vector<IDXGIAdapter*>adapters;
//ここに特定の名前を持つアダプターオブジェクトが入る
IDXGIAdapter* tmpAdapter = nullptr;

for(int i=0;
	dxgiFactory->EnumAdapters(i,&tmpAdapter)!=DXGI_ERROR_NOT_FOUND;i++)
{
	adapters.push_back(tmpAdapter);//動的配列に追加する
}

for (int i = 0; i < adapters.size(); i++) 
{
	DXGI_ADAPTER_DESC adesc{};
	adapters[i]->GetDesc(&adesc);//アダプターの情報を取得
	std::wstring strDesc = adesc.Description;//アダプター名
	//Microsoft Basic Render Driverを回避
	if (strDesc.find(L"Microsoft") == std::wstring::npos) 
	{
		tmpAdapter = adapters[i];//採用
		break;
	}
}

//デバイス
//デバイスはDirect3D12の基本オブジェクト　必須

//対応レベルの配列
D3D_FEATURE_LEVEL levels[] =
{
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
};

D3D_FEATURE_LEVEL featureLevel;

for (int i = 0; i < _countof(levels); i++)
{
	//採用したアラプターでデバイスを生成
	result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
	if (result == S_OK) 
	{
		//デバイスを生成できた時点でループを抜ける
		featureLevel = levels[i];
		break;
	}
}

//コマンドリスト
//GPUにまとめて命令を送るためのリスト

//コマンドアロケータを生成
result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	IID_PPV_ARGS(&cmdAllocator));
//コマンドリストを生成
result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
	cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));

//コマンドキュー
//コマンドリストをGPUに順に実行させていくための仕組み

//標準設定でコマンドキューを生成
D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

//各種設定をしてスワップチェーンを生成
DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
swapchainDesc.Width = 1280;
swapchainDesc.Height = 720;
swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色情報の書式
swapchainDesc.SampleDesc.Count = 1;//マルチサンプルしない
swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//バックバッファ用
swapchainDesc.BufferCount = 2;//バッファ数を2つに設定
swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は削除
swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
dxgiFactory->CreateSwapChainForHwnd(
	cmdQueue,
	hwnd,
	&swapchainDesc,
	nullptr,
	nullptr,
	(IDXGISwapChain1**)&swapchain);

//レンダーターゲットビュー
//バックバッファに描画結果を書き込む仕組み　バッファが2つあるなら2つ必要

//各所設定をしてディスクリプタヒープを生成
D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー
heapDesc.NumDescriptors = 2;//裏表の二つ
//裏表の2つ分について
std::vector<ID3D12Resource*>backBuffers(2);
for (int i = 0; i < 2; i++) 
{
	//スワップチェーンからバッファを取得
	result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
	//ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle =
		rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	//裏か表化でアドレスがずれる
	handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	//レンダーターゲットビュー生成
	dev->CreateRenderTargetView(
		backBuffers[i],
		nullptr,
		handle);
}

//フェンスの生成 CPUとGPUで同期をとるための仕組み

//フェンスの生成
ID3D12Fence* fence = nullptr;
UINT64 fenceVal = 0;
result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//初期化処理ここまで

	MSG msg{};//メッセ―ジ

	while (true) {
		//メッセージがある？
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);//キー入力メッセージの処理
			DispatchMessage(&msg);//プロs-じゃにメッセージを送る
		}

		//終了メッセージが来たらループを抜ける
		if (msg.message == WM_QUIT) {
			break;
		}

		//DirectX　毎フレーム処理ここから
		
		//リソースバリア
		//バックバッファなど、リソースの状態を確実に切り替えるための仕組み
		//1.描画先のバッファを、描画できる状態に切り替えるコマンド。
		//2.画面クリアコマンド。
		//3.描画コマンド
		//4.描画後にバッファを表示用の状態に戻すコマンド。
		//の順でコマンドリストに追加する

		//バックバッファの番号を取得(2つなので0番か1番)
		UINT bbindex = swapchain->GetCurrentBackBufferIndex();
		//1.リソースバリアを変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbindex];//バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//表示から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画
		cmdList->ResourceBarrier(1, &barrierDesc);

		//2.画面クリアコマンドここから
		//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
			rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr = bbindex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);
		//全画面クリア　　　　　R　　G　　B　　A
		float clearColor[] = { 0.1f,0.25f,0.5f,0.0f };//青っぽい色
		cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		//2.画面クリアコマンドここまで
		
		//3.描画コマンドここから
		//命令のクローズ
		cmdList->Close();
		//コマンドリストの実行
		ID3D12CommandList* cmdLists[] = { cmdList };//コマンドリストの配列
		cmdQueue->ExecuteCommandLists(1, cmdLists);
		//コマンドリストの実行完了を待つ
		cmdQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		cmdAllocator->Reset();//キューをクリア
		cmdList->Reset(cmdAllocator, nullptr);//再びコマンドリストをためる準備
		//バッファをフリップ(裏表の差し替え)
		swapchain->Present(1, 0);
		//3.描画コマンドここまで

		//4.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//表示に
		cmdList->ResourceBarrier(1, &barrierDesc);



		//DirectX　毎フレーム処理ここまで

	}

	//ウィンドウクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}