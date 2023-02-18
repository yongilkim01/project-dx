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
	/// Graphics class 생성자
	/// chain swap 객체 생성 및 설정을 해주고 해당 정보를 통해서 초기화
	/// </summary>
	/// <param name="hWnd"></param>

	/*
	* Direct Swap Chain Desc
	* 화면을 그리는데 깜빡이는 현상을 없애기 위해서 더블 버퍼링을 하게 된다
	* 화면 하나의 프레임 전체를 화면에 출력하는 것이 아닌 예비 버퍼에 그림을 그린 후(back buffer)
	* 예비 버퍼에 그린 그림을 화면에 출력하고 다음에 출력할 프레임 전체를 그린다(front buffer)
	* Swap chain 이란 front buffer와 back buffer간 포인터의 전환을 의미
	*/

	DXGI_SWAP_CHAIN_DESC _sd = {};
	_sd.BufferDesc.Width = 0; // 해상도 너비를 설정, 0으로 지정 시 윈도우 창의 너비로 설정
	_sd.BufferDesc.Height = 0; // 해상도 높이를 설정, 0으로 지정 시 윈도우 창의 높이로 설정
	_sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 표시 형식을 각 색과 알파당 8bit
	_sd.BufferDesc.RefreshRate.Numerator = 0; // 재생률의 분자
	_sd.BufferDesc.RefreshRate.Denominator = 0; // 재생률의 분모
	// 지정된 모니터의 해상도에 맞게 이미지가 늘어나는 기능을 지정하지 않는 것으로 설정
	_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// 표면에 이미지를 생성하는 순서를 지정하지 않는 것으로 설정
	_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_sd.SampleDesc.Count = 1; // 다중 샘플링을 하지 않는다
	_sd.SampleDesc.Quality = 0; // 다중 샘플링을 하지 않는다
	_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 랜더링
	_sd.BufferCount = 1; // 일단 더블 버퍼링을 하지 않게 설정을 한다
	_sd.OutputWindow = hWnd;
	_sd.Windowed = TRUE; // 창모드로 설정
	_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 교환 후 백버퍼를 버리기 위해 설정
	_sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Direct3D 함수들을 체크할 목적
	HRESULT hr;

	/*
	* 디바이스란 어셈블리어 명령 즉, 개발자가 작성한 코드를 토대로 그래픽카드 메모리에
	* 접근을 해서 할당 후 화면에 렌더링을 하게 해주는 역할
	*/
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr, // nullptr을 전달시 열거된 첫 번째 기본 어댑터를 사용해서 디바이스를 생성
		D3D_DRIVER_TYPE_HARDWARE, // 하드웨어에서 Direct3D 기능을 구현하는 기본 드라이버
		nullptr, // 위 항목인 드라이버 타입이 D3D_DRIVER_TYPE_SOFTWARE인 경우 nullptr이 아니어야 하지만 지금은 nullptr
		swapCreateFlags,
		nullptr, // GPU 기능 집합을 기본 설정으로 지정
		0,
		D3D11_SDK_VERSION, // SDK 버전은 D3D11_SDK_VERSION 으로 설정
		&_sd, // swap chain 초기화 매개 변수의 포인터
		&g_pSwapChain, // 렌더링에 사용되는 swap chain 변수의 포인터
		&g_pd3dDevice, // 디바이스의 포인터
		nullptr, // 기능 수준 배열의 첫번째 요소의 포인터를 반환 단 기능 수준을 결정할 필요가 없는 경우 nullptr로 입력
		&g_pImmediateContext // deivce context의 포인터 주소
	));

	// 리소스를 받을 COM 인터페이스를 생성
	ID3D11Resource* _pBackBuffer = nullptr;
	// swapchain 객체를 통해서 백 버퍼중 하낭 엑세스
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