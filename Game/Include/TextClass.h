#ifndef TEXTCLASS_H
#define TEXTCLASS_H

#pragma comment (lib, "d3dcompiler.lib")

#include <d3dcompiler.h>
#include "fontclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct vertexConstantBufferType
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewProjectionMatrix;
	};

	struct pixelConstantBufferType
	{
		XMFLOAT4 color;
	};

	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT4 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	FontClass* m_Font;

	int m_screenWidth, m_screenHeight;

	SentenceType* m_sentence1;

	ID3D11Buffer *vertexCBuffer, *pixelCBuffer;
	ID3D11PixelShader* fontPixelShader;

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, HWND _hwnd, int _width, int _height);
	bool Render(ID3D11DeviceContext* _deviceContext, const XMMATRIX &_worldMatrix, const XMMATRIX &_viewMatrix, const XMMATRIX &_orthographicMatrix);

	void Update(ID3D11DeviceContext* _deviceContext, const char* text);

	void Shutdown();

private:
	bool InitializeSentence(ID3D11Device* _device, SentenceType** _sentence, int _maxLength);

	bool UpdateSentence(ID3D11DeviceContext* _deviceContext, SentenceType* _sentence, const char* _text, int _posX, int _posY, float _red, float _green, float _blue);
	bool RenderSentence(ID3D11DeviceContext* _deviceContext, SentenceType* _sentence, const XMMATRIX &_worldMatrix, const XMMATRIX &_viewMatrix, const XMMATRIX &_orthographicMatrix);

	void ReleaseSentence(SentenceType** _sentence);
};

#endif