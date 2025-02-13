#define STB_IMAGE_IMPLEMENTATION

#include "GraphicsSetup.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <DirectXMath.h>

#include "stb_image.h"
#include "ConstantBuffersSetup.h"

namespace DX = DirectX;

// Function to read file content into a string
static bool readFile(const std::string& filePath, std::string& fileData) {
	std::ifstream reader(filePath, std::ios::binary | std::ios::ate);
	if (!reader.is_open()) {
		std::cerr << "Could not open file: " << filePath << std::endl;
		return false;
	}

	std::streamsize size = reader.tellg();
	reader.seekg(0, std::ios::beg);

	fileData.resize(size);
	if (!reader.read(&fileData[0], size)) {
		std::cerr << "Failed to read file: " << filePath << std::endl;
		reader.close();
		return false;
	}

	reader.close();
	return true;
}

// Function to load vertex and pixel shaders
static bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vsByteCode) {
	std::string shaderData;

	// Load Vertex Shader
	if (!readFile("VertexShader.cso", shaderData)) {
		return false;
	}

	// Create Vertex Shader
	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader))) {
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vsByteCode = shaderData; // Store bytecode for input layout creation
	shaderData.clear();

	// Load Pixel Shader
	if (!readFile("PixelShader.cso", shaderData)) {
		return false;
	}

	// Create Pixel Shader
	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader))) {
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;
}

// Function to create input layout
static bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode) {
	// Define input layout description
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create input layout
	HRESULT hr = device->CreateInputLayout(inputDesc, sizeof(inputDesc) / sizeof(*inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);
	return !FAILED(hr);
}

// Function to create vertex buffer
static bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer) {
	// Define vertices
	SimpleVertex vertices[4] =
	{
		{{-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0}},
		{{0.5f, 0.5f, 0.0f}, {0, 0, -1}, {1, 0}},
		{{-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1}},
		{{0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1}},
	};

	// Define buffer description
	D3D11_BUFFER_DESC bufferDesc = {
		bufferDesc.ByteWidth = sizeof(vertices),
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE,
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		bufferDesc.CPUAccessFlags = 0,
		bufferDesc.MiscFlags = 0,
		bufferDesc.StructureByteStride = 0
	};

	// Define subresource data
	D3D11_SUBRESOURCE_DATA data = {
		data.pSysMem = vertices,
		data.SysMemPitch = 0,
		data.SysMemSlicePitch = 0
	};

	// Create vertex buffer
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	return !FAILED(hr);
}

// Function to create texture and shader resource view
static bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, unsigned char*& imageData) {
	int width, height, channels;

	// Load image data
	imageData = stbi_load("image.jpg", &width, &height, &channels, 0);
	channels = 4; // Force 4 channels (RGBA)

	// Prepare texture data
	std::vector<unsigned char> textureData;
	textureData.resize(height * width * channels);

	int index = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			unsigned int startPos = (x + width * y) * channels;

			textureData[startPos + 0] = imageData[index++];
			textureData[startPos + 1] = imageData[index++];
			textureData[startPos + 2] = imageData[index++];
			textureData[startPos + 3] = 255; // Set alpha to 255
		}
	}

	stbi_image_free(imageData);
	imageData = nullptr;

	// Define texture description
	D3D11_TEXTURE2D_DESC textureDesc = {
		textureDesc.Width = static_cast<UINT>(width),
		textureDesc.Height = static_cast<UINT>(height),
		textureDesc.MipLevels = 1,
		textureDesc.ArraySize = 1,
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		textureDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 },
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE,
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		textureDesc.CPUAccessFlags = 0,
		textureDesc.MiscFlags = 0
	};

	// Define subresource data
	D3D11_SUBRESOURCE_DATA textureSubData = {
		textureSubData.pSysMem = &textureData[0],
		textureSubData.SysMemPitch = static_cast<UINT>(width * channels),
		textureSubData.SysMemSlicePitch = 0
	};

	// Create texture
	if (FAILED(device->CreateTexture2D(&textureDesc, &textureSubData, &texture))) {
		std::cerr << "Failed to create texture!" << std::endl;
		return false;
	}

	// Create shader resource view
	HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &srv);
	return !FAILED(hr);
}

// Function to create sampler state
static bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
	// Define sampler description
	D3D11_SAMPLER_DESC samplerDesc = {
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC,
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		samplerDesc.MipLODBias = 0,
		samplerDesc.MaxAnisotropy = 16,
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
		samplerDesc.BorderColor[0] = 0,
		samplerDesc.BorderColor[1] = 0,
		samplerDesc.BorderColor[2] = 0,
		samplerDesc.BorderColor[3] = 0,
		samplerDesc.MinLOD = 0.0f,
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX
	};


	// Create sampler state
	HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	return !(FAILED(hr));
}

// Function to set up the graphics pipeline
bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader,
	ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture,
	ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& samplerState, unsigned char*& imageData)
{
	std::string vsByteCode;

	// Load shaders
	if (!LoadShaders(device, vShader, pShader, vsByteCode)) {
		std::cerr << "Error loading shaders!" << std::endl;
		return false;
	}

	// Create input layout
	if (!CreateInputLayout(device, inputLayout, vsByteCode)) {
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	// Create vertex buffer
	if (!CreateVertexBuffer(device, vertexBuffer)) {
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}

	// Create texture and shader resource view
	if (!CreateTexture(device, texture, srv, imageData)) {
		std::cerr << "Error creating texture!" << std::endl;
		return false;
	}

	// Create sampler state
	if (!CreateSamplerState(device, samplerState)) {
		std::cerr << "Error creating sampler state!" << std::endl;
		return false;
	}

	return true;
}
