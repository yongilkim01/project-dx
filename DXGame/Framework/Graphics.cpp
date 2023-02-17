#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	/// <summary>
	/// Graphics class ������
	/// chain swap ��ü ���� �� ������ ���ְ� �ش� ������ ���ؼ� �ʱ�ȭ
	/// </summary>
	/// <param name="hWnd"></param>

	/*
	* Direct Swap Chain Desc
	* ȭ���� �׸��µ� �����̴� ������ ���ֱ� ���ؼ� ���� ���۸��� �ϰ� �ȴ�
	* ȭ�� �ϳ��� ������ ��ü�� ȭ�鿡 ����ϴ� ���� �ƴ� ���� ���ۿ� �׸��� �׸� ��(back buffer)
	* ���� ���ۿ� �׸� �׸��� ȭ�鿡 ����ϰ� ������ ����� ������ ��ü�� �׸���(front buffer)
	* Swap chain �̶� front buffer�� back buffer�� �������� ��ȯ�� �ǹ�
	*/

	DXGI_SWAP_CHAIN_DESC _sd = {};
	_sd.BufferDesc.Width = 0; // �ػ� �ʺ� ����, 0���� ���� �� ������ â�� �ʺ�� ����
	_sd.BufferDesc.Height = 0; // �ػ� ���̸� ����, 0���� ���� �� ������ â�� ���̷� ����
	_sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // ǥ�� ������ �� ���� ���Ĵ� 8bit
	_sd.BufferDesc.RefreshRate.Numerator = 0; // ������� ����
	_sd.BufferDesc.RefreshRate.Denominator = 0; // ������� �и�
	// ������ ������� �ػ󵵿� �°� �̹����� �þ�� ����� �������� �ʴ� ������ ����
	_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// ǥ�鿡 �̹����� �����ϴ� ������ �������� �ʴ� ������ ����
	_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_sd.SampleDesc.Count = 1; // ���� ���ø��� ���� �ʴ´�
	_sd.SampleDesc.Quality = 0; // ���� ���ø��� ���� �ʴ´�
	_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �ĸ� ���ۿ� ������
	_sd.BufferCount = 1; // �ϴ� ���� ���۸��� ���� �ʰ� ������ �Ѵ�
	_sd.OutputWindow = hWnd;
	_sd.Windowed = TRUE; // â���� ����
	_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // ��ȯ �� ����۸� ������ ���� ����
	_sd.Flags = 0;

	/*
	* ����̽��� ������� ��� ��, �����ڰ� �ۼ��� �ڵ带 ���� �׷���ī�� �޸𸮿�
	* ������ �ؼ� �Ҵ� �� ȭ�鿡 �������� �ϰ� ���ִ� ����
	*/
	D3D11CreateDeviceAndSwapChain(
		nullptr, // nullptr�� ���޽� ���ŵ� ù ��° �⺻ ����͸� ����ؼ� ����̽��� ����
		D3D_DRIVER_TYPE_HARDWARE, // �ϵ����� Direct3D ����� �����ϴ� �⺻ ����̹�
		nullptr, // �� �׸��� ����̹� Ÿ���� D3D_DRIVER_TYPE_SOFTWARE�� ��� nullptr�� �ƴϾ�� ������ ������ nullptr
		0,
		nullptr, // GPU ��� ������ �⺻ �������� ����
		0,
		D3D11_SDK_VERSION, // SDK ������ D3D11_SDK_VERSION ���� ����
		&_sd, // swap chain �ʱ�ȭ �Ű� ������ ������
		&g_pSwapChain, // �������� ���Ǵ� swap chain ������ ������
		&g_pd3dDevice, // ����̽��� ������
		nullptr, // ��� ���� �迭�� ù��° ����� �����͸� ��ȯ �� ��� ������ ������ �ʿ䰡 ���� ��� nullptr�� �Է�
		&g_pImmediateContext // deivce context�� ������ �ּ�
	);

	// ���ҽ��� ���� COM �������̽��� ����
	ID3D11Resource* _pBackBuffer = nullptr;
	// swapchain ��ü�� ���ؼ� �� ������ �ϳ� ������
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&_pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(
		_pBackBuffer,
		nullptr,
		&g_pRenderTargetView
	);
	_pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (g_pRenderTargetView != nullptr)
	{
		g_pRenderTargetView->Release();
	}
	if (g_pImmediateContext != nullptr)
	{
		g_pImmediateContext->Release();
	}
	if (g_pSwapChain != nullptr)
	{
		g_pSwapChain->Release();
	}
	if (g_pd3dDevice != nullptr)
	{
		g_pd3dDevice->Release();
	}
}

void Graphics::EndFrame()
{
	g_pSwapChain->Present(1u, 0u);
}