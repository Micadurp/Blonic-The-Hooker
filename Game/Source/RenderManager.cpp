#include"RenderManager.h"

RenderManager::RenderManager()
{
	basicModelVertexLayout = nullptr;
	basicModelVertexShader = nullptr;
	basicModelGeometryShader = nullptr;
	basicModelPixelShader = nullptr;
	basicModelVSCB = nullptr;
	basicModelGSCB = nullptr;
	basicModelPSCB = nullptr;
	defaultSampler = nullptr;
}

RenderManager::~RenderManager()
{

}

bool RenderManager::Initialize(ID3D11Device* _device, const DirectX::XMMATRIX &_projectionMatrix,int _screenWidth, int _screenHeight)
{
	HRESULT result;

#pragma region Create Basic Shaders

	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(L"BasicModelVertexShader.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, NULL, &pVS, nullptr);

	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &basicModelVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	_device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &basicModelVertexLayout);
	pVS->Release();

	//create Geometry shader
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(L"BasicModelGeometryShader.hlsl", NULL, nullptr, "GS_main", "gs_5_0", 0, NULL, &pGS, nullptr);

	_device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &basicModelGeometryShader);
	pGS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(L"BasicModelPixelShader.hlsl", NULL, nullptr, "PS_main", "ps_5_0", 0, NULL, &pPS, nullptr);

	_device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &basicModelPixelShader);
	pPS->Release();


	//create crosshair pixel shader
	ID3DBlob* chPS = nullptr;
	D3DCompileFromFile(L"crosshairPixelShader.hlsl", NULL, nullptr, "PS_main", "ps_5_0", 0, NULL, &chPS, nullptr);

	_device->CreatePixelShader(chPS->GetBufferPointer(), chPS->GetBufferSize(), nullptr, &crosshairPixelShader);
	chPS->Release();

	//create Hookstring Geometry shader
	ID3DBlob* hGS = nullptr;
	D3DCompileFromFile(L"HookStringGeometryShader.hlsl", NULL, nullptr, "GS_main", "gs_5_0", 0, NULL, &hGS, nullptr);

	_device->CreateGeometryShader(hGS->GetBufferPointer(), hGS->GetBufferSize(),nullptr, &hookStringGeometryShader);
	hGS->Release();


#pragma endregion

#pragma region Create ConstantBuffers

	D3D11_BUFFER_DESC matrixBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(VertexShaderBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = _device->CreateBuffer(&matrixBufferDesc, NULL, &basicModelVSCB);
	if (FAILED(result))
	{
		return false;
	}
#pragma endregion



	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampleDesc.MaxAnisotropy = 1;


	result = _device->CreateSamplerState(&sampleDesc, &defaultSampler);
	if (FAILED(result))
	{
		return false;
	}


	deferredRenderer = new DeferredRendering();
	if (!deferredRenderer)
	{
		return false;
	}
	deferredRenderer->Initilize(_device, _projectionMatrix, _screenWidth, _screenHeight);
	
	return true;
}

void RenderManager::Shutdown()
{
	if (defaultSampler)
	{
		defaultSampler->Release();
		defaultSampler = 0;
	}

	if (deferredRenderer)
	{
		deferredRenderer->Shutdown();
		delete deferredRenderer;
		deferredRenderer = 0;
	}

	if (crosshairPixelShader)
	{
		crosshairPixelShader->Release();
		crosshairPixelShader = 0;
	}

	// Release the vertex constant buffer
	if (basicModelVSCB)
	{
		basicModelVSCB->Release();
		basicModelVSCB = 0;
	}

	// Release the geometry constant buffer
	if (basicModelGSCB)
	{
		basicModelGSCB->Release();
		basicModelGSCB = 0;
	}

	// Release the pixel constant buffer
	if (basicModelPSCB)
	{
		basicModelPSCB->Release();
		basicModelPSCB = 0;
	}

	// Release the layout.
	if (basicModelVertexLayout)
	{
		basicModelVertexLayout->Release();
		basicModelVertexLayout = 0;
	}

	// Release the pixel shader.
	if (basicModelPixelShader)
	{
		basicModelPixelShader->Release();
		basicModelPixelShader = 0;
	}

	// Release the geometry shader.
	if (basicModelGeometryShader)
	{
		basicModelGeometryShader->Release();
		basicModelGeometryShader = 0;
	}

	// Release the vertex shader.
	if (basicModelVertexShader)
	{
		basicModelVertexShader->Release();
		basicModelVertexShader = 0;
	}


}

bool RenderManager::SameShader()
{
	return false;
}

bool RenderManager::SetShader(ID3D11DeviceContext* _deviceContext)
{

	// Set the vertex input layout.
	_deviceContext->IASetInputLayout(basicModelVertexLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	_deviceContext->VSSetShader(basicModelVertexShader, NULL, 0);
	_deviceContext->GSSetShader(basicModelGeometryShader, NULL, 0);
	_deviceContext->PSSetShader(basicModelPixelShader, NULL, 0);

	_deviceContext->PSSetSamplers(0, 1, &defaultSampler);

	return true;
}

void RenderManager::SetCrosshairShaders(ID3D11DeviceContext* _deviceContext)
{

	// Set the vertex input layout.
	_deviceContext->IASetInputLayout(basicModelVertexLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	_deviceContext->VSSetShader(basicModelVertexShader, NULL, 0);
	_deviceContext->GSSetShader(basicModelGeometryShader, NULL, 0);
	_deviceContext->PSSetShader(crosshairPixelShader, NULL, 0);
}
void RenderManager::SetHookStringShaders(ID3D11DeviceContext* _deviceContext)
{

	// Set the vertex input layout.
	_deviceContext->IASetInputLayout(basicModelVertexLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	_deviceContext->VSSetShader(basicModelVertexShader, NULL, 0);
	_deviceContext->GSSetShader(hookStringGeometryShader, NULL, 0);
	_deviceContext->PSSetShader(crosshairPixelShader, NULL, 0);
}


bool RenderManager::SetDeferredShaders(ID3D11DeviceContext* _devicecontext)
{
	return deferredRenderer->SetShaders(_devicecontext);
}

bool RenderManager::SetVertexCBuffer(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix, const DirectX::XMMATRIX &_projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexShaderBuffer* dataPtr;

	result = _deviceContext->Map(basicModelVSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (VertexShaderBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = DirectX::XMMatrixTranspose(_worldMatrix);
	dataPtr->viewProjection = DirectX::XMMatrixTranspose(_viewMatrix * _projectionMatrix);

	// Unlock the constant buffer.
	_deviceContext->Unmap(basicModelVSCB, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	_deviceContext->VSSetConstantBuffers(0, 1, &basicModelVSCB);

	return true;
}


void RenderManager::DeferredFirstPass(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView * _depthStencilView)
{
	_deviceContext->PSSetSamplers(0, 1, &defaultSampler);
	deferredRenderer->FirstPass(_deviceContext, _depthStencilView);
}

void RenderManager::DeferredRenderer(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView * _depthStencilView, ID3D11RenderTargetView * backBuffer)
{
	deferredRenderer->Render(_deviceContext, _depthStencilView, backBuffer);
}