#include "D3D11Helper.h"

// Function to create D3D11 device, device context, and swap chain
static bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain) {
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {
		swapChainDesc.BufferDesc.Width = width,
		swapChainDesc.BufferDesc.Height = height,
		swapChainDesc.BufferDesc.RefreshRate = { 0, 1 },
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		swapChainDesc.SampleDesc = { 1, 0 },
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		swapChainDesc.BufferCount = 1,
		swapChainDesc.OutputWindow = window,
		swapChainDesc.Windowed = true,
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		swapChainDesc.Flags = 0
	};

	UINT flags = 0;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	// Create device and swap chain
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &immediateContext);
	return !FAILED(hr);
}

// Function to create render target view
static bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv) {
	ID3D11Texture2D* backBuffer = nullptr;

	// Get back buffer from swap chain
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
		std::cerr << "Failed to get back buffer!" << std::endl;
		return false;
	}

	// Create render target view
	HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
	backBuffer->Release();
	return !FAILED(hr);
}

// Function to create depth stencil texture and view
static bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView) {
	D3D11_TEXTURE2D_DESC textureDesc = {
		textureDesc.Width = width,
		textureDesc.Height = height,
		textureDesc.MipLevels = 1,
		textureDesc.ArraySize = 1,
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		textureDesc.SampleDesc = { 1, 0 },
		textureDesc.Usage = D3D11_USAGE_DEFAULT,
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		textureDesc.CPUAccessFlags = 0,
		textureDesc.MiscFlags = 0
	};

	// Create depth stencil texture
	if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture))) {
		std::cerr << "Failed to create depth stencil texture!" << std::endl;
		return false;
	}

	// Create depth stencil view
	HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
	return !FAILED(hr);
}

// Function to set viewport dimensions
static void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height) {
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
}

// Function to set up D3D11 pipeline
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport)
{
	// Create device, context, and swap chain
	if (!CreateInterfaces(width, height, window, device, immediateContext, swapChain)) {
		std::cerr << "Error creating interfaces!" << std::endl;
		return false;
	}

	// Create render target view
	if (!CreateRenderTargetView(device, swapChain, rtv)) {
		std::cerr << "Error creating rtv!" << std::endl;
		return false;
	}

	// Create depth stencil texture and view
	if (!CreateDepthStencil(device, width, height, dsTexture, dsView)) {
		std::cerr << "Error creating depth stencil view!" << std::endl;
		return false;
	}

	// Set viewport dimensions
	SetViewport(viewport, width, height);

	return true;
}