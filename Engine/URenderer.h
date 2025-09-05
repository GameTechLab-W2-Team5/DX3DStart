#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "UCamera.h"

class URenderer
{
private:
    // Core D3D11 objects
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11RasterizerState* rasterizerState;

    // Shader objects
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    // Depth stencil
    ID3D11DepthStencilState* depthStencilState;

    // Constant buffer
    ID3D11Buffer* constantBuffer;

    // Viewport
    D3D11_VIEWPORT viewport;

    // Camera
    UCamera* currentCamera;

    // Window handle
    HWND hWnd;

    // Render state
    bool bIsInitialized;

public:
    URenderer();
    ~URenderer();

    // Initialization and cleanup
    bool Create(HWND windowHandle);
    bool CreateShader();
    bool CreateRasterizerState();
    bool CreateDepthStencilState();
    bool CreateConstantBuffer();
    void Release();
    void ReleaseShader();
    void ReleaseConstantBuffer();

    // Buffer creation
    ID3D11Buffer* CreateVertexBuffer(const void* data, size_t sizeInBytes);
    ID3D11Buffer* CreateIndexBuffer(const void* data, size_t sizeInBytes);
    bool ReleaseVertexBuffer(ID3D11Buffer* buffer);
    bool ReleaseIndexBuffer(ID3D11Buffer* buffer);

    // Texture creation
    ID3D11Texture2D* CreateTexture2D(int width, int height, DXGI_FORMAT format,
        const void* data = nullptr);
    ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* texture);
    bool ReleaseTexture(ID3D11Texture2D* texture);
    bool ReleaseShaderResourceView(ID3D11ShaderResourceView* srv);

    // Rendering operations
    void Prepare();
    void PrepareShader();
    void SwapBuffer();
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    // Drawing operations
    void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
    void Draw(UINT vertexCount, UINT startVertexLocation = 0);
    void DrawMesh(UMesh* mesh);

    // Resource binding
    void SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset = 0);
    void SetIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
    void SetConstantBuffer(ID3D11Buffer* buffer, UINT slot = 0);
    void SetTexture(ID3D11ShaderResourceView* srv, UINT slot = 0);

    // Set Active Camera
    void SetActiveCamera(UCamera* camera) { currentCamera = camera; }
    UCamera* GetCurrentCamera() { return currentCamera; }

    // Constant buffer updates
    bool UpdateConstantBuffer(const void* data, size_t sizeInBytes);

    // Window resize handling
    bool ResizeBuffers(int width, int height);

    // Getters
    ID3D11Device* GetDevice() const { return device; }
    ID3D11DeviceContext* GetDeviceContext() const { return deviceContext; }
    IDXGISwapChain* GetSwapChain() const { return swapChain; }
    bool IsInitialized() const { return bIsInitialized; }

    // Utility functions
    bool CheckDeviceState();
    void GetBackBufferSize(int& width, int& height);

private:
    // Internal helper functions
    bool CreateDeviceAndSwapChain(HWND windowHandle);
    bool CreateRenderTargetView();
    bool CreateDepthStencilView(int width, int height);
    bool SetupViewport(int width, int height);

    // Error handling
    void LogError(const char* function, HRESULT hr);
    bool CheckResult(HRESULT hr, const char* function);
};