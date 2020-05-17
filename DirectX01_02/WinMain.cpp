#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//�������ɕK�v�ȕϐ�����
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

//�A�_�v�^�̗�
//

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
	}
}

//�f�o�C�X
//�f�o�C�X��Direct3D12�̊�{�I�u�W�F�N�g�@�K�{

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

//�R�}���h���X�g
//GPU�ɂ܂Ƃ߂Ė��߂𑗂邽�߂̃��X�g

//�R�}���h�A���P�[�^�𐶐�
result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	IID_PPV_ARGS(&cmdAllocator));
//�R�}���h���X�g�𐶐�
result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
	cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));

//�R�}���h�L���[
//�R�}���h���X�g��GPU�ɏ��Ɏ��s�����Ă������߂̎d�g��

//�W���ݒ�ŃR�}���h�L���[�𐶐�
D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

//�e��ݒ�����ăX���b�v�`�F�[���𐶐�
DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
swapchainDesc.Width = 1280;
swapchainDesc.Height = 720;
swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�F���̏���
swapchainDesc.SampleDesc.Count = 1;//�}���`�T���v�����Ȃ�
swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//�o�b�N�o�b�t�@�p
swapchainDesc.BufferCount = 2;//�o�b�t�@����2�ɐݒ�
swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͍폜
swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
dxgiFactory->CreateSwapChainForHwnd(
	cmdQueue,
	hwnd,
	&swapchainDesc,
	nullptr,
	nullptr,
	(IDXGISwapChain1**)&swapchain);

//�����_�[�^�[�Q�b�g�r���[
//�o�b�N�o�b�t�@�ɕ`�挋�ʂ��������ގd�g�݁@�o�b�t�@��2����Ȃ�2�K�v

//�e���ݒ�����ăf�B�X�N���v�^�q�[�v�𐶐�
D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[
heapDesc.NumDescriptors = 2;//���\�̓��
//���\��2���ɂ���
std::vector<ID3D12Resource*>backBuffers(2);
for (int i = 0; i < 2; i++) 
{
	//�X���b�v�`�F�[������o�b�t�@���擾
	result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
	//�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE handle =
		rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	//�����\���ŃA�h���X�������
	handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	//�����_�[�^�[�Q�b�g�r���[����
	dev->CreateRenderTargetView(
		backBuffers[i],
		nullptr,
		handle);
}

//�t�F���X�̐��� CPU��GPU�œ������Ƃ邽�߂̎d�g��

//�t�F���X�̐���
ID3D12Fence* fence = nullptr;
UINT64 fenceVal = 0;
result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

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
		
		//���\�[�X�o���A
		//�o�b�N�o�b�t�@�ȂǁA���\�[�X�̏�Ԃ��m���ɐ؂�ւ��邽�߂̎d�g��
		//1.�`���̃o�b�t�@���A�`��ł����Ԃɐ؂�ւ���R�}���h�B
		//2.��ʃN���A�R�}���h�B
		//3.�`��R�}���h
		//4.�`���Ƀo�b�t�@��\���p�̏�Ԃɖ߂��R�}���h�B
		//�̏��ŃR�}���h���X�g�ɒǉ�����

		//�o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
		UINT bbindex = swapchain->GetCurrentBackBufferIndex();
		//1.���\�[�X�o���A��ύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbindex];//�o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//�\������
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`��
		cmdList->ResourceBarrier(1, &barrierDesc);

		//2.��ʃN���A�R�}���h��������
		//�����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
			rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr = bbindex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);
		//�S��ʃN���A�@�@�@�@�@R�@�@G�@�@B�@�@A
		float clearColor[] = { 0.1f,0.25f,0.5f,0.0f };//���ۂ��F
		cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		//2.��ʃN���A�R�}���h�����܂�
		
		//3.�`��R�}���h��������
		//���߂̃N���[�Y
		cmdList->Close();
		//�R�}���h���X�g�̎��s
		ID3D12CommandList* cmdLists[] = { cmdList };//�R�}���h���X�g�̔z��
		cmdQueue->ExecuteCommandLists(1, cmdLists);
		//�R�}���h���X�g�̎��s������҂�
		cmdQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		cmdAllocator->Reset();//�L���[���N���A
		cmdList->Reset(cmdAllocator, nullptr);//�ĂуR�}���h���X�g�����߂鏀��
		//�o�b�t�@���t���b�v(���\�̍����ւ�)
		swapchain->Present(1, 0);
		//3.�`��R�}���h�����܂�

		//4.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`��
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//�\����
		cmdList->ResourceBarrier(1, &barrierDesc);



		//DirectX�@���t���[�����������܂�

	}

	//�E�B���h�E�N���X��o�^����
	UnregisterClass(w.lpszClassName, w.hInstance);
}