#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>

/// <summary>
/// Sets up Direct3D 11 with the specified parameters.
/// </summary>
/// <param name="width">- Width of the window.</param>
/// <param name="height">- Height of the window.</param>
/// <param name="window">- Handle to the window.</param>
/// <param name="device">- Reference to the Direct3D device.</param>
/// <param name="immediateContext">- Reference to the immediate device context.</param>
/// <param name="swapChain">- Reference to the swap chain.</param>
/// <param name="rtv">- Reference to the render target view.</param>
/// <param name="dsTexture">- Reference to the depth-stencil texture.</param>
/// <param name="dsView">- Reference to the depth-stencil view.</param>
/// <param name="viewport">- Reference to the viewport.</param>
/// <returns>True if Direct3D 11 was successfully set up, otherwise false.</returns>
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain,
	ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport);