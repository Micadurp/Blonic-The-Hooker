#include"SkyBox.h"

SkyBox::SkyBox()
{

}

SkyBox::SkyBox(ID3D11Device * _device)
{
	Initialize(L"sphere", _device);

	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(L"SkyBoxVertexShader.hlsl", NULL, nullptr, "VS_main", "vs_4_0", 0, NULL, &pVS, nullptr);

	HRESULT result  = _device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &skyVertexShader);
	pVS->Release();
	if (!result)
		true;

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(L"SkyBoxPixelShader.hlsl", NULL, nullptr, "PS_main", "ps_4_0", 0, NULL, &pPS, nullptr);

	_device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &skyPixelShader);
	pPS->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

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
	result = _device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	if (!result)
		true;
}

SkyBox::~SkyBox()
{

}
void SkyBox::Update(Camera* camera)
{
	XMMATRIX scale;
	XMMATRIX translation;

	XMLoadFloat4x4(&objMatrix) = XMMatrixIdentity();
	scale = XMMatrixScaling(20, 20, 20);
	translation = XMMatrixTranslation(XMVectorGetX(XMLoadFloat4(&camera->GetPosition())), XMVectorGetY(XMLoadFloat4(&camera->GetPosition())), XMVectorGetZ(XMLoadFloat4(&camera->GetPosition())));
	
	XMStoreFloat4x4(&objMatrix,scale * translation);
}

void SkyBox::Render(ID3D11DeviceContext* _deviceContext,ID3D11DepthStencilState * _depthState)
{
	_deviceContext->OMSetDepthStencilState(depthStencilState, 1);

 	_deviceContext->VSSetShader(skyVertexShader, NULL, 0);
	_deviceContext->GSSetShader(NULL, NULL, 0);
	_deviceContext->PSSetShader(skyPixelShader, NULL, 0);

	Model::Render(_deviceContext, _depthState);

	_deviceContext->OMSetDepthStencilState(_depthState, 1);

}