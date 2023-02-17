#pragma once
#include "DxWin.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0f };
		g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, color);
	}
private:
	ID3D11Device* g_pd3dDevice = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11DeviceContext* g_pImmediateContext = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
};