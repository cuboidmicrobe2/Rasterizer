#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "GraphicsSetup.h"

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer) {
	// BG
	float clearColor[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColor);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	// Number of vertices to draw
	immediateContext->Draw(6, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;

	// Window Setup
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window)) {
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;

	// D3D11 Setup
	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport)) {
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	// Pipeline Setup
	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout)) {
		std::cerr << "Failed to seutp pipeline!" << std::endl;
		return -1;
	}

	MSG msg = {};

	// Window Loop
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer);
		swapChain->Present(0, 0);
	}

	// Freeing of objects
	device->Release();
	immediateContext->Release();
	swapChain->Release();
	rtv->Release();
	dsTexture->Release();
	dsView->Release();
	vShader->Release();
	pShader->Release();
	inputLayout->Release();
	vertexBuffer->Release();

	return 0;
}