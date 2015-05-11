#include "Direct3D.h"


Direct3D::Direct3D()
{

}

Direct3D::~Direct3D()
{

}

bool Direct3D::Initialize(int _screenWidth, int _screenHeight, bool _vsync, HWND _hwnd, bool _fullscreen, float _screenDepth, float _screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;
	D3D11_BLEND_DESC blendStateDescription;

	// Store the vsync setting.
	vsync_enabled = _vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)_screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)_screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;


	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = _screenWidth;
	swapChainDesc.BufferDesc.Height = _screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to. 
	swapChainDesc.OutputWindow = _hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (_fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//// Set the scan line ordering and scaling to unspecified.
	//swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//// Discard the back buffer contents after presenting.
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		NULL,
		&deviceContext);

	if (FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &backBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;


	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = _screenWidth;
	depthBufferDesc.Height = _screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;


	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}


	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &backBuffer, depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	deviceContext->RSSetState(rasterState);

	// Setup the viewport for rendering.
	viewport.Width = (float)_screenWidth;
	viewport.Height = (float)_screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)_screenWidth / (float)_screenHeight;

	// Create the projection matrix for 3D rendering.
	DirectX::XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, _screenNear, _screenDepth));

	// Create an orthographic projection matrix for 2D rendering.
	DirectX::XMStoreFloat4x4(&orthoMatrix, XMMatrixOrthographicLH((float)_screenWidth, (float)_screenHeight, _screenNear, _screenDepth));

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	renderer = new RenderManager();
	if (!renderer)
	{
		return false;
	}
	result = renderer->Initialize(device,XMLoadFloat4x4(&projectionMatrix), _screenWidth, _screenHeight);

	return true;
}

void Direct3D::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}
		
	if (alphaEnableBlendingState)
	{
		alphaEnableBlendingState->Release();
		alphaEnableBlendingState = 0;
	}

	if (alphaDisableBlendingState)
	{
		alphaDisableBlendingState->Release();
		alphaDisableBlendingState = 0;
	}

	if (rasterState)
	{
		rasterState->Release();
		rasterState = 0;
	}

	if (renderer)
	{
		renderer->Shutdown();
		delete renderer;
		renderer = 0;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = 0;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if (backBuffer)
	{
		backBuffer->Release();
		backBuffer = 0;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}

	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}

	return;
}

void Direct3D::BeginScene( float _red, float _green, float _blue, float _alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = _red;
	color[1] = _green;
	color[2] = _blue;
	color[3] = _alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(backBuffer, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//deviceContext->OMSetRenderTargets(1, &backBuffer, depthStencilView);

	
	renderer->SetShader(deviceContext);

	renderer->DeferredFirstPass(deviceContext, depthStencilView);

	return;
}

void Direct3D::EndScene()
{
	renderer->DeferredRenderer(deviceContext, depthStencilView, backBuffer);

	// Present the back buffer to the screen since rendering is complete.
	if (vsync_enabled)
	{
		// Lock to screen refresh rate.
		swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		swapChain->Present(0, 0);
	}
	return;
}

ID3D11Device* Direct3D::GetDevice()
{
	return device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
	return deviceContext;
}

bool Direct3D::SetShader()
{
	return renderer->SetShader(deviceContext);
}

bool Direct3D::SetDeferredShaders()
{
	return renderer->SetDeferredShaders(deviceContext);
}

void Direct3D::SetCrosshairShaders()
{
	renderer->SetCrosshairShaders(deviceContext);
}

bool Direct3D::SetPixelCBuffer(ID3D11Buffer** _lightBuffers, const LightPosColor &_lights, const LightSharedInfo &_lightInfo)
{
	return renderer->SetPixelCBuffer(deviceContext, _lightBuffers, _lights, _lightInfo);
}

bool Direct3D::SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix)
{
	return renderer->SetVertexCBuffer(deviceContext, _worldMatrix, XMMatrixIdentity(), XMMatrixIdentity());
}

bool Direct3D::SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix)
{
	return renderer->SetVertexCBuffer(deviceContext, _worldMatrix, _viewMatrix, XMLoadFloat4x4(&projectionMatrix));
}

bool Direct3D::SetVertexCBuffer(const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix, const DirectX::XMMATRIX &_projectionMatrix)
{
	return renderer->SetVertexCBuffer(deviceContext, _worldMatrix, _viewMatrix, _projectionMatrix);
}

void Direct3D::SetBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1, &backBuffer, depthStencilView);
}

bool Direct3D::Render(std::vector<Model*> &_models, const DirectX::XMMATRIX &_viewMatrix)
{
	for (int n = 0; n < _models.size(); n++)
	{
		SetVertexCBuffer(_models[n]->GetObjMatrix(), _viewMatrix);
		_models.at(n)->Render(deviceContext,depthStencilState);
	}

	return true;
}

bool Direct3D::Render(Model* _model, const DirectX::XMMATRIX &_viewMatrix)
{
	SetVertexCBuffer(_model->GetObjMatrix(), _viewMatrix);
	_model->Render(deviceContext, depthStencilState);

	return true;
}

void Direct3D::ResetViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
}

ID3D11DepthStencilView * Direct3D::GetDepthStencilView()
{
	return depthStencilView;
}

 ID3D11RenderTargetView * Direct3D::GetBackBufferRenderTarget()
{
	 return backBuffer;
}

XMMATRIX Direct3D::GetProjectionMatrix()
{
	return DirectX::XMLoadFloat4x4(&projectionMatrix);
}


XMMATRIX Direct3D::GetOrthoMatrix()
{
	return DirectX::XMLoadFloat4x4(&orthoMatrix);
}

void Direct3D::GetVideoCardInfo(char* _cardName, int& _memory)
{
	strcpy_s(_cardName, 128, videoCardDescription);
	_memory = videoCardMemory;
	return;
}

void Direct3D::TurnOnAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.75f;
	blendFactor[1] = 0.75f;
	blendFactor[2] = 0.75f;
	blendFactor[3] = 1.0f;

	deviceContext->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void Direct3D::TurnOffAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.75f;
	blendFactor[1] = 0.75f;
	blendFactor[2] = 0.75f;
	blendFactor[3] = 1.0f;

	deviceContext->OMSetBlendState(alphaDisableBlendingState, blendFactor, 0xffffffff);
}