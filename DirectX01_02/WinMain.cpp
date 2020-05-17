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

//DXGI�t�@�N�g���[�̐���
result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
//�A�_�v�^�[�̗񋓗p
std::vector<IDXGIAdapter*>adapters;
//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
IDXGIAdapter* tmpAdapter = nullptr;

for(int i=0;
	dxgiFactory->EnumAdapters(i,&tmpAdapter)!=dxgi_ERROR_NOT_FOUND;i++)
{
	adapters.push_back(tmpAdapter);//���I�z��ɒǉ�����
}

for (int i = 0; i < adapters.size(); I++) 
{
	DXGI_ADAPTER_DESC adesc{};
	adapters[i]->GetDesc(&adesc);//�A�_�v�^�[�̏����擾
	std::wstring strDesc = adesc.Description;//�A�_�v�^�[��
	//Microsoft Basic Render Driver�����
	if (stdDesc.find(L"Microsoft") == std::wstring::npos) 
	{
		tmpAdapter = adapters[i];//�̗p
		break;
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