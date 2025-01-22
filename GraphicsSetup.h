#pragma once

#include <array>
#include <d3d11.h>

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

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader,
	ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture, 
	ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& samplerState, unsigned char*& imageData);