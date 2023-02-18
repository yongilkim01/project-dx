#pragma once
#include "DxWin.h"
#include "DxException.h"
#include "InfoManager.h"

#include <vector>
#include <d3d11.h>

class Graphics
{
public:
	class Exception : public DxException
	{
		using DxException::DxException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int _line, const char* _file, HRESULT _hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
#ifndef NDEBUG
	InfoManager infoManager;
#endif
	ID3D11Device* g_pd3dDevice = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11DeviceContext* g_pImmediateContext = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
};