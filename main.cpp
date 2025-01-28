#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <chrono>

#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "GraphicsSetup.h"
#include "ConstantBuffersSetup.h"

namespace DX = DirectX;

// Render function to draw the scene
static void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
	ID3D11ShaderResourceView* srv, ID3D11SamplerState* samplerState) {

	// Clear the render target and depth stencil views
	float clearColor[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColor);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	// Set the vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the input layout and primitive topology
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set the vertex and pixel shaders
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(pShader, nullptr, 0);

	// Set the shader resources and samplers
	immediateContext->PSSetShaderResources(0, 1, &srv);
	immediateContext->PSSetSamplers(0, 1, &samplerState);

	// Set the viewport and render targets
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	// Draw the vertices
	immediateContext->Draw(4, 0);
}

// Update rotation based on elapsed time
static void UpdateRotation(float& rotation) {
	static auto prevTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - prevTime;
	prevTime = currentTime;

	rotation += deltaTime.count();
	if (rotation > 360) rotation = 0;
}

// Main entry point for the application
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Window Setup
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window)) {
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	// Interface variables
	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;
	ID3D11VertexShader* vShader;
	ID3D11Buffer* vBuffer;
	ID3D11PixelShader* pShader;
	ID3D11Buffer* pBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	unsigned char* imageData;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* samplerState;

	// D3D11 Setup
	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport)) {
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	// Pipeline Setup
	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, texture, srv, samplerState, imageData)) {
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	// Setup constant buffers for vertex and pixel shader
	float rotation = 300.0f;
	DX::XMFLOAT4X4 matrixArray[2]{};
	if (!SetupConstantBuffers(device, WIDTH, HEIGHT, rotation, vBuffer, pBuffer, matrixArray)) {
		std::cerr << "Failed to setup constant buffer!" << std::endl;
		return -1;
	}
	immediateContext->VSSetConstantBuffers(0, 1, &vBuffer);
	immediateContext->PSSetConstantBuffers(0, 1, &pBuffer);

	// Window Loop
	MSG msg = {};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UpdateRotation(rotation);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		immediateContext->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		DX::XMStoreFloat4x4(&matrixArray[0], CreateWorldMatrix(rotation));
		memcpy(mappedResource.pData, matrixArray, sizeof(matrixArray));
		immediateContext->Unmap(vBuffer, 0);

		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, srv, samplerState);
		swapChain->Present(0, 0);
	}

	// Release resources
	samplerState->Release();
	srv->Release();
	texture->Release();
	vertexBuffer->Release();
	inputLayout->Release();
	pBuffer->Release();
	vBuffer->Release();
	pShader->Release();
	vShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	delete[] imageData;

	return 0;
}