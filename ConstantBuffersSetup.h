#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

/// <summary>
/// Creates a world matrix for a given rotation angle.
/// </summary>
/// <param name="angle">- The rotation angle in radians.</param>
/// <returns>A world matrix representing the rotation.</returns>
DirectX::XMMATRIX CreateWorldMatrix(const float& angle);

/// <summary>
/// Sets up the constant buffers for the vertex and pixel shaders.
/// </summary>
/// <param name="device">- The Direct3D device.</param>
/// <param name="WIDTH">- The width of the viewport.</param>
/// <param name="HEIGHT">- The height of the viewport.</param>
/// <param name="rotation">- The rotation angle in radians.</param>
/// <param name="vBuffer">- A reference to the vertex buffer.</param>
/// <param name="pBuffer">- A reference to the pixel buffer.</param>
/// <param name="matrixArray">- An array of matrices to be used in the shaders.</param>
/// <returns>True if the buffers were set up successfully, false otherwise.</returns>
bool SetupConstantBuffers(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT, const float rotation,
	ID3D11Buffer*& vBuffer, ID3D11Buffer*& pBuffer, DirectX::XMFLOAT4X4 matrixArray[2]);