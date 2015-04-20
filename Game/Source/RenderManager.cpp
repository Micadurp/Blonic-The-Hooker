#include"RenderManager.h"

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{
	
}

bool RenderManager::Initilize(ID3D11Device* device)
{
	HRESULT result;

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

	D3D11_BUFFER_DESC matrixBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(VertexShaderBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &basicModelVSCB);
	if (FAILED(result))
	{
		return false;
	}
#pragma endregion

}

void RenderManager::Shutdown()
{
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

bool RenderManager::SetShader(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &worldMatrix, const DirectX::XMMATRIX &viewMatrix, const DirectX::XMMATRIX &projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexShaderBuffer* dataPtr;

	result = deviceContext->Map(basicModelVSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (VertexShaderBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = DirectX::XMMatrixTranspose(worldMatrix);
	dataPtr->viewProjection = DirectX::XMMatrixTranspose(viewMatrix * projectionMatrix);

	// Unlock the constant buffer.
	deviceContext->Unmap(basicModelVSCB, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &basicModelVSCB);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(basicModelVertexLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(basicModelVertexShader, NULL, 0);
	deviceContext->GSSetShader(basicModelGeometryShader, NULL, 0);
	deviceContext->PSSetShader(basicModelPixelShader, NULL, 0);
}