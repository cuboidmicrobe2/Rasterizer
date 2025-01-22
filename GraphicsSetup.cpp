#define STB_IMAGE_IMPLEMENTATION

#include "GraphicsSetup.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "stb_image.h"

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
        {{-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0}},
        {{0.5f, 0.5f, 0.0f}, {0, 0, -1}, {1, 0}},
        {{-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1}},

        // 4th vertex
        {{0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1}},
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

bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, unsigned char*& imageData) {
    int height, width, channels;

    imageData = stbi_load("image.jpg", &width, &height, &channels, 0);
    channels = 4;

    std::vector<unsigned char> textureData;
    textureData.resize(height * width * channels);

    int index = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned int startPos = (x + width * y) * channels;

            textureData[startPos + 0] = imageData[index++];
            textureData[startPos + 1] = imageData[index++];
            textureData[startPos + 2] = imageData[index++];
            textureData[startPos + 3] = 255;
        }
    }

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA textureSubData;
    textureSubData.pSysMem = &textureData[0];
    textureSubData.SysMemPitch = width * channels;
    textureSubData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateTexture2D(&textureDesc, &textureSubData, &texture))) {
        std::cerr << "Failed to create texture!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &srv);
    return !FAILED(hr);
}

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    return !(FAILED(hr));
}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader,
    ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture,
    ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& samplerState, unsigned char*& imageData)
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

    if (!CreateTexture(device, texture, srv, imageData)) {
        std::cerr << "Error creating texture!" << std::endl;
        return false;
    }

    if (!CreateSamplerState(device, samplerState)) {
        std::cerr << "Error creating sampler state!" << std::endl;
        return false;
    }

    return true;
}
