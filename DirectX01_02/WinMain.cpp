#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

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
		//a

	}
}

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



		//DirectX　毎フレーム処理ここまで

	}

	//ウィンドウクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}