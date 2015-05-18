#ifndef DIRECT3D_H
#define DIRECT3D_H

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "RenderManager.h"
#include"Model.h"

#include <vector>

using namespace DirectX;

class Direct3D
{
private:
	bool vsync_enabled;
	int videoCardMemory;
	char videoCardDescription[128];
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* backBuffer;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 orthoMatrix;

	D3D11_VIEWPORT viewport;
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11BlendState* alphaDisableBlendingState;

	RenderManager* renderer;
public:
	Direct3D();
	~Direct3D();

	bool Initialize(int _screenWidth, int _screenHeight, bool _vsync, HWND _hwnd, bool _fullscreen, float _screenDepth, float _screenNear);
	void Shutdown();

	void BeginScene(float _red, float _green, float _blue, float _alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void SetCrosshairShaders();
	bool SetShader();
	bool SetDeferredShaders();

	bool SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix);
	bool SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix);
	bool SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix, const DirectX::XMMATRIX &_projectionMatrix);

	bool Render(std::vector<Model*> &_models, const DirectX::XMMATRIX &_viewMatrix);
	bool Render(Model*, const DirectX::XMMATRIX &_viewMatrix);

	void DeferredFirstPass();
	void DeferredRender();

	void SetBackBufferRenderTarget();
	void ResetViewport();

	ID3D11DepthStencilView * GetDepthStencilView();
	ID3D11RenderTargetView * GetBackBufferRenderTarget();

	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();

	void GetVideoCardInfo(char*, int&);

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

};

#endif