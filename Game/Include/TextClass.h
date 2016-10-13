#ifndef TEXTCLASS_H
#define TEXTCLASS_H

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")

#include <dxgi.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d2d1.h>
#include <sstream>
#include <dwrite.h>
#include <DirectXMath.h>


using namespace std;


class TextClass
{
	private:
		struct VertexType
		{
			VertexType()
			{
				position = { 0.0f, 0.0f, 0.0f, 0.0f };
				texCoord = { 0.0f, 0.0f };
				normal = { 0.0f, 0.0f, 0.0f };
			}

			VertexType(float x, float y, float z,
				float u, float v,
				float nx, float ny, float nz) : position(x, y, z, 1.0f), texCoord(u, v), normal(nx, ny, nz) {}

			DirectX::XMFLOAT4 position;
			DirectX::XMFLOAT2 texCoord;
			DirectX::XMFLOAT3 normal;
		};

		struct VSconstantBufferType
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4X4 viewProjection;
		} vertexCBufferObj;

		ID3D11Buffer* textCBuffer;

		ID3D10Device1 *d3d101Device;
		IDXGIKeyedMutex *keyedMutex11;
		IDXGIKeyedMutex *keyedMutex10;
		ID2D1RenderTarget *d2dRenderTarget;
		ID2D1SolidColorBrush *brush;
		ID3D11Texture2D *sharedTex11;
		ID3D11Buffer *d2dVertBuffer;
		ID3D11Buffer *d2dIndexBuffer;
		ID3D11ShaderResourceView *d2dTexture;
		IDWriteFactory *dwriteFactory;
		IDWriteTextFormat *textFormat;

		wstring printText;

		int width, height;

	public:
		TextClass();
		~TextClass();

		bool Initialize(ID3D11Device* _device, IDXGIAdapter1* adapter, int _width, int _height);

		void Update(double _time);
		void Render(ID3D11DeviceContext* _deviceContext);
		void ShutDown();

	private:
		bool InitializeD2DSquare(ID3D11Device* _device);
};

#endif