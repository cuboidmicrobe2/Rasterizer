#include "ConstantBuffersSetup.h"
#include <Windows.h>
#include <iostream>

namespace DX = DirectX;

// Function to create the world matrix
DX::XMMATRIX CreateWorldMatrix(const float& angle) {
	using namespace DX;
	XMMATRIX translationMatrix = XMMatrixTranslation(0, 0, -1);
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
	return XMMatrixTranspose(XMMatrixMultiply(translationMatrix, rotationMatrix));
}

// Function to create the view matrix
static DX::XMMATRIX CreateViewMatrix() {
	using namespace DX;
	FXMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
	FXMVECTOR focusPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	FXMVECTOR upPosition = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	return XMMatrixLookAtLH(eyePosition, focusPosition, upPosition);
}

// Function to create the projection matrix
static DX::XMMATRIX CreateProjectionMatrix(float fovAngle, float aspectRatio, float nearZ, float farZ) {
	using namespace DX;
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngle), aspectRatio, nearZ, farZ);
}

// Function to create world, view, and projection matrices
static void CreateMatrices(const UINT WIDTH, const UINT HEIGHT, const float rotation, DX::XMFLOAT4X4*& matrixArray) {
	DX::XMMATRIX worldMatrix = CreateWorldMatrix(rotation);
	DX::XMMATRIX viewMatrix = CreateViewMatrix();
	float fovAngle = 59.0f;
	float aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
	float nearZ = 0.1f;
	float farZ = 100.0f;
	DX::XMMATRIX projectionMatrix = CreateProjectionMatrix(fovAngle, aspectRatio, nearZ, farZ);
	DX::XMMATRIX viewProjMatrix = DX::XMMatrixMultiplyTranspose(viewMatrix, projectionMatrix);

	// Store matrices in XMFLOAT4X4 array
	DX::XMStoreFloat4x4(&matrixArray[0], worldMatrix);
	DX::XMStoreFloat4x4(&matrixArray[1], viewProjMatrix);
}

// Function to create vertex shader constant buffer
static bool CreateVSConstBuffer(ID3D11Device* device, DX::XMFLOAT4X4 matrixArray[2], ID3D11Buffer*& buffer) {
	D3D11_BUFFER_DESC vsConstBuffer = {
		.ByteWidth = sizeof(DX::XMFLOAT4X4[2]),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};

	D3D11_SUBRESOURCE_DATA vsSubResource = { .pSysMem = matrixArray };

	HRESULT hr = device->CreateBuffer(&vsConstBuffer, &vsSubResource, &buffer);
	return !FAILED(hr);
}

// Function to create pixel shader constant buffer
static bool CreatePSConstBuffer(ID3D11Device* device, ID3D11Buffer*& buffer) {
	struct psStruct {
		DX::XMFLOAT4 lightPosition = { 0.0f, 0.5f, -5.0f, 1.0f };
		DX::XMFLOAT4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DX::XMFLOAT4 cameraPosition = { 0.0f, 0.0f, -3.0f, 1.0f };
		float ambientLightIntensity = 0.01f;
		float shininess = 50.0f;
		char padding[8];
	} psVars;

	D3D11_BUFFER_DESC psConstBuffer = {
		.ByteWidth = sizeof(psStruct),
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER
	};

	D3D11_SUBRESOURCE_DATA psSubResource = { .pSysMem = &psVars };

	HRESULT hr = device->CreateBuffer(&psConstBuffer, &psSubResource, &buffer);
	if (FAILED(hr)) {
		std::cerr << "Creation of pixel buffer failed!" << std::endl;
		return false;
	}
	return true;
}

// Function to set up constant buffers for vertex and pixel shaders
bool SetupConstantBuffers(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT, const float rotation,
	ID3D11Buffer*& vBuffer, ID3D11Buffer*& pBuffer, DirectX::XMFLOAT4X4 matrixArray[2])
{
	// Create world, view, and projection matrices
	CreateMatrices(WIDTH, HEIGHT, rotation, matrixArray);

	// Create vertex shader constant buffer
	if (!CreateVSConstBuffer(device, matrixArray, vBuffer)) {
		std::cerr << "Failed creating vertex shader constant buffer!" << std::endl;
		return false;
	}

	// Create pixel shader constant buffer
	if (!CreatePSConstBuffer(device, pBuffer)) {
		std::cerr << "Failed creating pixel shader constant buffer!" << std::endl;
		return false;
	}

	return true;
}
