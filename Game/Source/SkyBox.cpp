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
}

SkyBox::~SkyBox()
{

}
void SkyBox::Update(Camera* camera)
{
	XMMATRIX scale;
	XMMATRIX translation;

	XMLoadFloat4x4(&objMatrix) = XMMatrixIdentity();
	scale = XMMatrixScaling(10, 10, 10);
	translation = XMMatrixTranslation(XMVectorGetX(XMLoadFloat4(&camera->GetPosition())), XMVectorGetY(XMLoadFloat4(&camera->GetPosition())), XMVectorGetZ(XMLoadFloat4(&camera->GetPosition())));
	
	XMStoreFloat4x4(&objMatrix,scale * translation);
}

void SkyBox::Render(ID3D11DeviceContext* _deviceContext)
{
 	_deviceContext->VSSetShader(skyVertexShader, NULL, 0);
	_deviceContext->GSSetShader(NULL, NULL, 0);
	_deviceContext->PSSetShader(skyPixelShader, NULL, 0);

	Model::Render(_deviceContext);

}