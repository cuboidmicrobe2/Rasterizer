#include "GraphicsSetup.h"

#include <string>
#include <fstream>
#include <iostream>

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode) {
    std::string shaderData;
    std::ifstream reader;

    // Vertex Shader
    reader.open("VertexShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open()) {
        std::cerr << "Could not open vertex shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader))) {
        std::cerr << "Failed to create vertex shader!" << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();

    // Pixel Shader
    reader.open("PixelShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open()) {
        std::cerr << "Could not open pixel shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader))) {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        return false;
    }

    return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode) {
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, sizeof(inputDesc) / sizeof(*inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}

bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer) {
    SimpleVertex vertices[4] =
    {
        // Triangle
        {{-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 1}},
        {{0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 1}},
        {{-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1}},

        // 4th vertex
        {{0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1}},
    };

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = vertices;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
    return !FAILED(hr);
}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout)
{
    std::string vShaderByteCode;

    if (!LoadShaders(device, vShader, pShader, vShaderByteCode)) {
        std::cerr << "Error loading shaders!" << std::endl;
        return false;
    }

    if (!CreateInputLayout(device, inputLayout, vShaderByteCode)) {
        std::cerr << "Error creating input layout!" << std::endl;
        return false;
    }

    if (!CreateVertexBuffer(device, vertexBuffer)) {
        std::cerr << "Error creating vertex buffer!" << std::endl;
        return false;
    }

    return true;
}
