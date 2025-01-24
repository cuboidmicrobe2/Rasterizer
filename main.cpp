#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <chrono>

#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "GraphicsSetup.h"

namespace DX = DirectX;

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv,
    ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
    ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
    ID3D11ShaderResourceView* srv, ID3D11SamplerState* samplerState) {

    // Clear background
    float clearColor[4] = { 0, 0, 0, 0 };
    immediateContext->ClearRenderTargetView(rtv, clearColor);
    immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    immediateContext->IASetInputLayout(inputLayout);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediateContext->VSSetShader(vShader, nullptr, 0);
    immediateContext->PSSetShader(pShader, nullptr, 0);
    immediateContext->RSSetViewports(1, &viewport);
    immediateContext->PSSetShaderResources(0, 1, &srv);
    immediateContext->PSSetSamplers(0, 1, &samplerState);
    immediateContext->OMSetRenderTargets(1, &rtv, dsView);

    // Draw vertices
    immediateContext->Draw(4, 0);
}

DX::XMMATRIX CreateWorldMatrix(float& angle) {
    using namespace DX;
    XMMATRIX translationMatrix = XMMatrixTranslation(0, 0, -1);
    XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
    return XMMatrixTranspose(XMMatrixMultiply(translationMatrix, rotationMatrix));
}

DX::XMMATRIX CreateViewMatrix() {
    using namespace DX;
    FXMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
    FXMVECTOR focusPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    FXMVECTOR upPosition = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
    return XMMatrixLookAtLH(eyePosition, focusPosition, upPosition);
}

DX::XMMATRIX CreateProjectionMatrix(float fovAngle, float aspectRatio, float nearZ, float farZ) {
    using namespace DX;
    return XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngle), aspectRatio, nearZ, farZ);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    const UINT WIDTH = 1024;
    const UINT HEIGHT = 576;
    HWND window;

    // Window Setup
    if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window)) {
        std::cerr << "Failed to setup window!" << std::endl;
        return -1;
    }

    ID3D11Device* device;
    ID3D11DeviceContext* immediateContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv;
    ID3D11Texture2D* dsTexture;
    ID3D11DepthStencilView* dsView;
    D3D11_VIEWPORT viewport;
    ID3D11VertexShader* vShader;
    ID3D11PixelShader* pShader;
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

    // Create matrices
    float rotation = 300.0f;
    DX::XMMATRIX worldMatrix = CreateWorldMatrix(rotation);
    DX::XMMATRIX viewMatrix = CreateViewMatrix();
    float fovAngle = 59.0f;
    float aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    float nearZ = 0.1f;
    float farZ = 100.0f;
    DX::XMMATRIX projectionMatrix = CreateProjectionMatrix(fovAngle, aspectRatio, nearZ, farZ);
    DX::XMMATRIX viewProjMatrix = DX::XMMatrixMultiplyTranspose(viewMatrix, projectionMatrix);

    // Convert matrices to XMFloat4x4
    DX::XMFLOAT4X4 worldFloat;
    DX::XMStoreFloat4x4(&worldFloat, worldMatrix);
    DX::XMFLOAT4X4 viewProjFloat;
    DX::XMStoreFloat4x4(&viewProjFloat, viewProjMatrix);
    DX::XMFLOAT4X4 matrixArray[2] = { worldFloat, viewProjFloat };

    // Vertex Shader Constant Buffer
    D3D11_BUFFER_DESC vsConstBuffer = {};
    vsConstBuffer.ByteWidth = sizeof(worldFloat) + sizeof(viewProjFloat);
    vsConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
    vsConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vsConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vsSubResource = {};
    vsSubResource.pSysMem = matrixArray;

    ID3D11Buffer* vsIBuffer;
    HRESULT hr = device->CreateBuffer(&vsConstBuffer, &vsSubResource, &vsIBuffer);
    if (FAILED(hr)) {
        std::cerr << "Creation of vertex buffer failed!" << std::endl;
        return -1;
    }

    struct psStruct {
        DX::XMFLOAT4 lightPosition = { 0.0f, 0.5f, -5.0f, 0.0f };
        DX::XMFLOAT4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        DX::XMFLOAT4 cameraPosition = { 0.0f, 0.0f, -3.0f, 1.0f };
        float ambientLightIntensity = 0.01f;
        float shininess = 100.0f;
        char padding[8];
    };

    psStruct psVars;

    D3D11_BUFFER_DESC psConstBuffer = {};
    psConstBuffer.ByteWidth = sizeof(psStruct);
    psConstBuffer.Usage = D3D11_USAGE_IMMUTABLE;
    psConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA psSubResource = {};
    psSubResource.pSysMem = &psVars;

    ID3D11Buffer* psIBuffer;
    hr = device->CreateBuffer(&psConstBuffer, &psSubResource, &psIBuffer);
    if (FAILED(hr)) {
        std::cerr << "Creation of pixel buffer failed!" << std::endl;
    }

    immediateContext->VSSetConstantBuffers(0, 1, &vsIBuffer);
    immediateContext->PSSetConstantBuffers(0, 1, &psIBuffer);

    auto prevTime = std::chrono::high_resolution_clock::now();
    MSG msg = {};

    // Window Loop
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - prevTime;
        prevTime = currentTime;

        rotation += deltaTime.count();
        if (rotation > 360) rotation = 0;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        immediateContext->Map(vsIBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        DX::XMStoreFloat4x4(&matrixArray[0], CreateWorldMatrix(rotation));
        memcpy(mappedResource.pData, matrixArray, sizeof(matrixArray));
        immediateContext->Unmap(vsIBuffer, 0);

        Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, srv, samplerState);
        swapChain->Present(0, 0);
    }

    // Release resources
    device->Release();
    immediateContext->Release();
    swapChain->Release();
    rtv->Release();
    dsTexture->Release();
    dsView->Release();
    vShader->Release();
    pShader->Release();
    inputLayout->Release();
    vertexBuffer->Release();
    srv->Release();
    samplerState->Release();
    delete[] imageData;

    return 0;
}