#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif


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

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Direct3D �Լ����� üũ�� ����
	HRESULT hr;

	/*
	* ����̽��� ������� ��� ��, �����ڰ� �ۼ��� �ڵ带 ���� �׷���ī�� �޸𸮿�
	* ������ �ؼ� �Ҵ� �� ȭ�鿡 �������� �ϰ� ���ִ� ����
	*/
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr, // nullptr�� ���޽� ���ŵ� ù ��° �⺻ ����͸� ����ؼ� ����̽��� ����
		D3D_DRIVER_TYPE_HARDWARE, // �ϵ����� Direct3D ����� �����ϴ� �⺻ ����̹�
		nullptr, // �� �׸��� ����̹� Ÿ���� D3D_DRIVER_TYPE_SOFTWARE�� ��� nullptr�� �ƴϾ�� ������ ������ nullptr
		swapCreateFlags,
		nullptr, // GPU ��� ������ �⺻ �������� ����
		0,
		D3D11_SDK_VERSION, // SDK ������ D3D11_SDK_VERSION ���� ����
		&_sd, // swap chain �ʱ�ȭ �Ű� ������ ������
		&g_pSwapChain, // �������� ���Ǵ� swap chain ������ ������
		&g_pd3dDevice, // ����̽��� ������
		nullptr, // ��� ���� �迭�� ù��° ����� �����͸� ��ȯ �� ��� ������ ������ �ʿ䰡 ���� ��� nullptr�� �Է�
		&g_pImmediateContext // deivce context�� ������ �ּ�
	));

	// ���ҽ��� ���� COM �������̽��� ����
	ID3D11Resource* _pBackBuffer = nullptr;
	// swapchain ��ü�� ���ؼ� �� ������ �ϳ� ������
	GFX_THROW_INFO(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&_pBackBuffer)));
	GFX_THROW_INFO(g_pd3dDevice->CreateRenderTargetView(_pBackBuffer, nullptr, &g_pRenderTargetView));
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
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = g_pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(g_pd3dDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, color);
}

Graphics::HrException::HrException(int _line, const char* _file, HRESULT _hr, std::vector<std::string> _infoMsgs) noexcept
	:
	Exception(_line, _file),
	hr(_hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : _infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "DirectX Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}


std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "DirectX Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}