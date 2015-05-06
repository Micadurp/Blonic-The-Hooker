#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


class DeferredRendering
{
private:
	ID3D11Buffer* meshVertBuff;
	ID3D11Buffer* indexBuffer;

	int nrOfRenderTargets;
	UINT32 vertexSize;

	ID3D11RenderTargetView** renderTargetView;
	ID3D11ShaderResourceView** shaderResourceView;

	ID3D11ShaderResourceView ** clearRV;

	struct Vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;

		Vertex()
		{
			position.x = 0;
			position.y = 0;
			position.z = 0;
			position.w = 0;

			texture.x = 0;
			texture.y = 0;

			normal.x = 0;
			normal.y = 0;
			normal.z = 0;
		}
		Vertex(float _x, float _y, float _z, float _U, float _V)
		{
			position.x = _x;
			position.y = _y;
			position.z = _z;
			position.w = 1;

			texture.x = _U;
			texture.y = _V;

			normal.x = 0;
			normal.y = 0;
			normal.z = 0;
		}
	};

	struct VertexCB
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewProjection;
	}
	vertexCB;

	ID3D11PixelShader * deferredPixelShader;

	ID3D11Buffer*   deferredCB; //Deferred vertex shader constant buffer


public:
	DeferredRendering();
	~DeferredRendering();

	void Initilize(ID3D11Device* _device, const DirectX::XMMATRIX &_projectionMatrix, int _screenWidth, int _screenHeight);

	void FirstPass(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView);
	void Render(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* gBackbufferRTV);
};
#endif