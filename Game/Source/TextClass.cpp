#include "textclass.h"

TextClass::TextClass()
{
	m_Font = 0;

	m_sentence1 = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, HWND _hwnd, int _screenWidth, int _screenHeight)
{
	bool result;
	

	// Store the screen width and height.
	m_screenWidth = _screenWidth;
	m_screenHeight = _screenHeight;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Initialize the font object.
	result = m_Font->Initialize(_device, "fontData.txt", L"fonts.dds");
	if (!result)
	{
		MessageBox(_hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}


	// Initialize the first sentence.
	result = InitializeSentence(_device, &m_sentence1, 16);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(_deviceContext, m_sentence1, "Hello", 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	ID3DBlob* pPS = nullptr;
	HRESULT hr = D3DCompileFromFile(L"FontPixelShader.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, NULL, &pPS, nullptr);
	if (FAILED(hr))
	{
		return false;
	}

	hr = _device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &fontPixelShader);
	if (FAILED(hr))
	{
		return false;
	}

	pPS->Release();


	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	bufferDesc.ByteWidth = sizeof(vertexConstantBufferType);
	hr = _device->CreateBuffer(&bufferDesc, NULL, &vertexCBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	bufferDesc.ByteWidth = sizeof(pixelConstantBufferType);
	hr = _device->CreateBuffer(&bufferDesc, NULL, &pixelCBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}


void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

void TextClass::Update(ID3D11DeviceContext* _deviceContext, const char* _text)
{
	UpdateSentence(_deviceContext, m_sentence1, _text, 100, 100, 1.0f, 1.0f, 1.0f);
}

bool TextClass::Render(ID3D11DeviceContext* _deviceContext, const XMMATRIX &_worldMatrix, const XMMATRIX &_viewMatrix, const XMMATRIX &_orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(_deviceContext, m_sentence1, _worldMatrix, _viewMatrix, _orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(ID3D11Device* _device, SentenceType** _sentence, int _maxLength)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Create a new sentence object.
	*_sentence = new SentenceType;
	if (!*_sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*_sentence)->vertexBuffer = 0;
	(*_sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*_sentence)->maxLength = _maxLength;

	// Set the number of vertices in the vertex array.
	(*_sentence)->vertexCount = 6 * _maxLength;

	// Set the number of indexes in the index array.
	(*_sentence)->indexCount = (*_sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*_sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*_sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*_sentence)->vertexCount));

	// Initialize the index array.
	for (int i = 0; i<(*_sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*_sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = _device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*_sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*_sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = _device->CreateBuffer(&indexBufferDesc, &indexData, &(*_sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(ID3D11DeviceContext* _deviceContext, SentenceType* _sentence, const char* _text, int _positionX, int _positionY, float _red, float _green, float _blue)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	_sentence->red = _red;
	_sentence->green = _green;
	_sentence->blue = _blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(_text);

	// Check for possible buffer overflow.
	if (numLetters > _sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[_sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * _sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)((m_screenWidth / 2) + _positionX);
	drawY = (float)((m_screenHeight / 2) - _positionY);
	

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, _text, drawX, drawY);
	

	// Lock the vertex buffer so it can be written to.
	result = _deviceContext->Map(_sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _sentence->vertexCount));

	// Unlock the vertex buffer.
	_deviceContext->Unmap(_sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** _sentence)
{
	if (*_sentence)
	{
		// Release the sentence vertex buffer.
		if ((*_sentence)->vertexBuffer)
		{
			(*_sentence)->vertexBuffer->Release();
			(*_sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*_sentence)->indexBuffer)
		{
			(*_sentence)->indexBuffer->Release();
			(*_sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *_sentence;
		*_sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* _deviceContext, SentenceType* _sentence, const XMMATRIX &_worldMatrix, const XMMATRIX &_viewMatrix, const XMMATRIX &_orthoMatrix)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11ShaderResourceView* tempTex = nullptr;
	vertexConstantBufferType dataPtr;
	pixelConstantBufferType dataPtr2;
	unsigned int bufferNumber;

	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;

	tempTex = m_Font->GetTexture();
	if (!tempTex)
	{
		return false;
	}


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &_sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(_sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = { _sentence->red, _sentence->green, _sentence->blue, 1.0f };

	// Transpose the matrices to prepare them for the shader.
	DirectX::XMStoreFloat4x4(&dataPtr.worldMatrix, XMMatrixTranspose(_worldMatrix));
	DirectX::XMStoreFloat4x4(&dataPtr.viewProjectionMatrix, XMMatrixTranspose(XMMatrixMultiply(_viewMatrix, _orthoMatrix)));

	dataPtr2.color = pixelColor;

	_deviceContext->UpdateSubresource(vertexCBuffer, NULL, NULL, &dataPtr, 0, 0);
	_deviceContext->UpdateSubresource(pixelCBuffer, NULL, NULL, &dataPtr2, 0, 0);


	// Set shaders
	_deviceContext->GSSetShader(NULL, NULL, 0);
	_deviceContext->PSSetShader(fontPixelShader, NULL, 0);

	// Set the constant buffer in the vertex shader with the updated values.
	_deviceContext->VSSetConstantBuffers(0, 1, &vertexCBuffer);

	// Set shader texture resource in the pixel shader.
	_deviceContext->PSSetShaderResources(0, 1, &tempTex);

	// Set the pixel constant buffer in the pixel shader with the updated value.
	_deviceContext->PSSetConstantBuffers(0, 1, &pixelCBuffer);

	// Render the triangles.
	_deviceContext->DrawIndexed(_sentence->indexCount, 0, 0);

	return true;
}