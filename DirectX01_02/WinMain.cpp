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
	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY://�E�B���h�E���j�����ꂽ
		PostQuitMessage(0);//OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�W���̏������s��

}

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//DirectX������������������

		//�E�B���h�E�T�C�Y
	const int window_width = 1280;//����
	const int window_height = 720;//�c��

	WNDCLASSEX w{};//�E�B���h�E�N���X�̐ݒ�
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//�E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);//�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���w��

	//�E�B���h�E�N���X��OS��o�^
	RegisterClassEx(&w);
	//�E�B���h�E�T�C�Y(x���W,y���W,����,�c��)
	RECT wrc = { 0,0,window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�����ŃT�C�Y�␳

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X��
		L"DirectXGame",                      //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,                 //�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,                       //�\��x���W(OS�ɔC����)
		CW_USEDEFAULT,                       //�\��y���W(OS�ɔC����)
		wrc.right - wrc.left,                //�E�B���h�E����
		wrc.bottom - wrc.top,                //�E�B���h�E�c��
		nullptr,                             //�e�E�B���h�E�n���h��
		nullptr,                             //���j���[�n���h��
		w.hInstance,                         //�Ăяo���A�v���P�[�V�����n���h��
		nullptr);                            //�I�v�V����

	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

//DXGI�t�@�N�g���[�̐���
result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
//�A�_�v�^�[�̗񋓗p
std::vector<IDXGIAdapter*>adapters;
//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
IDXGIAdapter* tmpAdapter = nullptr;

for(int i=0;
	dxgiFactory->EnumAdapters(i,&tmpAdapter)!=DXGI_ERROR_NOT_FOUND;i++)
{
	adapters.push_back(tmpAdapter);//���I�z��ɒǉ�����
}

for (int i = 0; i < adapters.size(); i++) 
{
	DXGI_ADAPTER_DESC adesc{};
	adapters[i]->GetDesc(&adesc);//�A�_�v�^�[�̏����擾
	std::wstring strDesc = adesc.Description;//�A�_�v�^�[��
	//Microsoft Basic Render Driver�����
	if (strDesc.find(L"Microsoft") == std::wstring::npos) 
	{
		tmpAdapter = adapters[i];//�̗p
		break;
		//a

	}
}

//�Ή����x���̔z��
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
	//�̗p�����A���v�^�[�Ńf�o�C�X�𐶐�
	result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
	if (result == S_OK) 
	{
		//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
		featureLevel = levels[i];
		break;
	}
}
	//���������������܂�

	MSG msg{};//���b�Z�\�W

	while (true) {
		//���b�Z�[�W������H
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);//�L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);//�v��s-����Ƀ��b�Z�[�W�𑗂�
		}

		//�I�����b�Z�[�W�������烋�[�v�𔲂���
		if (msg.message == WM_QUIT) {
			break;
		}

		//DirectX�@���t���[��������������



		//DirectX�@���t���[�����������܂�

	}

	//�E�B���h�E�N���X��o�^����
	UnregisterClass(w.lpszClassName, w.hInstance);
}