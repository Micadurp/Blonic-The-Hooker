#include "textclass.h"

TextClass::TextClass()
{
}


TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* _device, IDXGIAdapter1* _adapter, int _width, int _height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC sharedDesc;
	IDXGIResource* sharedResource10;
	HANDLE sharedHandle10;
	IDXGISurface1* sharedSurface10;
	ID2D1Factory* d2dFactory;
	D2D1_RENDER_TARGET_PROPERTIES rtProperties;
	D3D11_BUFFER_DESC textCBufferDesc;

	width = _width;
	height = _height;



	// Create direct3D 10.1 device
	hr = D3D10CreateDevice1(_adapter,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_1_SDK_VERSION,
		&d3d101Device);
	if (FAILED(hr))
	{
		return false;
	}


	// Create shared texture direct3D 10.1 will render on
	ZeroMemory(&sharedDesc, sizeof(D3D11_TEXTURE2D_DESC));

	sharedDesc.Width = width;
	sharedDesc.Height = height;
	sharedDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedDesc.MipLevels = 1;
	sharedDesc.ArraySize = 1;
	sharedDesc.SampleDesc.Count = 1;
	sharedDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = _device->CreateTexture2D(&sharedDesc, NULL, &sharedTex11);
	if (FAILED(hr))
	{
		return false;
	}


	// Get the keyed mutex for the shared texture (for d3d11)
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);
	if (FAILED(hr))
	{
		return false;
	}


	// Get the shared handle needed to open the shared texture in d3d10.1
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	if (FAILED(hr))
	{
		return false;
	}

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	if (FAILED(hr))
	{
		return false;
	}

	sharedResource10->Release();


	// Open the surface for the shared texture in d3d10.1
	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)&sharedSurface10);
	if (FAILED(hr))
	{
		return false;
	}

	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);
	if (FAILED(hr))
	{
		return false;
	}


	// CReate D2D factory
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&d2dFactory);
	if (FAILED(hr))
	{
		return false;
	}


	ZeroMemory(&rtProperties, sizeof(D2D1_RENDER_TARGET_PROPERTIES));

	rtProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	rtProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = d2dFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &rtProperties, &d2dRenderTarget);
	if (FAILED(hr))
	{
		return false;
	}

	sharedResource10->Release();
	d2dFactory->Release();


	// Create a solid color brush to draw something with
	hr = d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &brush);
	if (FAILED(hr))
	{
		return false;
	}


	// DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));
	if (FAILED(hr))
	{
		return false;
	}

	hr = dwriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&textFormat);
	if (FAILED(hr))
	{
		return false;
	}

	hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if (FAILED(hr))
	{
		return false;
	}

	hr = textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (FAILED(hr))
	{
		return false;
	}

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);


	// Create constant buffer
	textCBufferDesc.ByteWidth = sizeof(VSconstantBufferType);
	textCBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	textCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	textCBufferDesc.MiscFlags = 0;
	textCBufferDesc.CPUAccessFlags = 0;

	hr = _device->CreateBuffer(&textCBufferDesc, NULL, &textCBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	bool res = InitializeD2DSquare(_device);
	if (!res)
	{
		return false;
	}



	return true;
}

bool TextClass::InitializeD2DSquare(ID3D11Device* _device)
{
	HRESULT result;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA iinitData;
	D3D11_SUBRESOURCE_DATA vertexBufferData;



	VertexType v[] = 
	{
		VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		VertexType( 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		VertexType( 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f)
	};

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	iinitData.pSysMem = indices;

	result = _device->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;

	result = _device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = _device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextClass::Update(double _time)
{
	// Create our string
	wostringstream ss;
	ss << _time;

	printText = ss.str();
}

void TextClass::Render(ID3D11DeviceContext* _deviceContext)
{
	// Release the d3d11 device
	keyedMutex11->ReleaseSync(0);

	// Use d3d10.1 device
	keyedMutex10->AcquireSync(0, 5);

	// Draw d2d content
	d2dRenderTarget->BeginDraw();

	// Clear d2d background
	d2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	// Set the Font color
	D2D1_COLOR_F fontColor = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the brush color D2D will use to draw with
	brush->SetColor(fontColor);
	
	// Create the d2d render area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, width, height);

	// Draw the text
	d2dRenderTarget->DrawTextW(printText.c_str(), wcslen(printText.c_str()), textFormat, layoutRect, brush);

	d2dRenderTarget->EndDraw();

	// Release the D3D10.1 device
	keyedMutex10->ReleaseSync(1);

	// Use the D3D11 device
	keyedMutex11->AcquireSync(1, 5);


	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the d2d Index buffer
	_deviceContext->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the d2d vertex buffer
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	
	DirectX::XMStoreFloat4x4(&vertexCBufferObj.world, DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
	DirectX::XMStoreFloat4x4(&vertexCBufferObj.viewProjection, DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));

	_deviceContext->UpdateSubresource(textCBuffer, 0, NULL, &vertexCBufferObj, 0, 0);

	_deviceContext->VSSetConstantBuffers(0, 1, &textCBuffer);
	_deviceContext->PSSetShaderResources(0, 1, &d2dTexture);

	
	//Draw the second cube
	_deviceContext->DrawIndexed(6, 0, 0);
}