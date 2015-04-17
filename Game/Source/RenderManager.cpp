#include"RenderManager.h"

RenderManager::RenderManager(ID3D11Device* device)
{
#pragma region Create Basic Shaders
	
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(L"BasicModelVertexShader.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, NULL, &pVS, nullptr);

	device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &basicModelVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &basicModelVertexLayout);
	pVS->Release();

	//create Geometry shader
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(L"BasicModelGeometryShader.hlsl", NULL, nullptr, "GS_main", "gs_5_0", 0, NULL, &pGS, nullptr); 

	device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &basicModelGeometryShader);
	pGS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(L"BasicModelPixelShader.hlsl", NULL, nullptr, "PS_main", "ps_5_0", 0, NULL, &pPS, nullptr);

	device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &basicModelPixelShader);
	pPS->Release();

#pragma endregion

#pragma region Create ConstantBuffers

#pragma endregion
}

RenderManager::~RenderManager()
{
	
}

bool RenderManager::SameShader()
{
	return false;
}

void RenderManager::SetShader(ID3D11DeviceContext* deviceContext)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(basicModelVertexLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(basicModelVertexShader, NULL, 0);
	deviceContext->GSSetShader(basicModelGeometryShader, NULL, 0);
	deviceContext->PSSetShader(basicModelPixelShader, NULL, 0);
}