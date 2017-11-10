#pragma once

#include<d3d11.h>
#include<D3DX11.h>
#include<d3dcompiler.h>

class DX11Base
{
public:
	DX11Base();
	virtual ~DX11Base();

	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	void Shutdown();

	bool CompileD3DShader(char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer);

	virtual bool LoadContent();
	virtual void UnloadContent();

	virtual void Update() = 0;
	virtual void Render() = 0;

protected:
	HINSTANCE hInstance_;
	HWND hwnd_;

	D3D_DRIVER_TYPE driverType_;
	D3D_FEATURE_LEVEL featureLevel_;

	ID3D11Device* d3dDevice_;
	ID3D11DeviceContext* d3dContext_;
	IDXGISwapChain* swapChain_;
	ID3D11RenderTargetView* backBufferTarget_;

	ID3D11Texture2D* depthTexture_;
	ID3D11DepthStencilView* depthStencilView_;
};

