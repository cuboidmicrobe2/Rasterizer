#pragma once

#include <array>
#include <d3d11.h>
#include <DirectXMath.h>

struct SimpleVertex {
	float pos[3];
	float rgb[3];
	float uv[2];

	SimpleVertex(const std::array<float, 3>& pos, const std::array<float, 3>& rgb, const std::array<float, 2>& uv) {
		for (int i = 0; i < 3; i++)
		{
			this->pos[i] = pos[i];
			this->rgb[i] = rgb[i];
		}

		this->uv[0] = uv[0];
		this->uv[1] = uv[1];
	}
};

/// <summary>
/// Sets up the graphics pipeline by creating and initializing the necessary Direct3D 11 resources.
/// </summary>
/// <param name="device">- The Direct3D device used to create resources.</param>
/// <param name="vertexBuffer">- Reference to the vertex buffer to be created.</param>
/// <param name="vShader">- Reference to the vertex shader to be created.</param>
/// <param name="pShader">- Reference to the pixel shader to be created.</param>
/// <param name="inputLayout">- Reference to the input layout to be created.</param>
/// <param name="texture">- Reference to the texture to be created.</param>
/// <param name="srv">- Reference to the shader resource view to be created.</param>
/// <param name="samplerState">- Reference to the sampler state to be created.</param>
/// <param name="imageData">- Pointer to the image data to be used for the texture.</param>
/// <returns>Returns true if the pipeline setup is successful, otherwise false.</returns>
bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader,
	ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture,
	ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& samplerState, unsigned char*& imageData);