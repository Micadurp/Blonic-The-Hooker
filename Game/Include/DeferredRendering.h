#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include <d3d11.h>
#include <d3dcompiler.h>

class DeferredRendering
{
private:
	ID3D11Buffer* meshVertBuff;

	int nrOfRenderTargets;
	UINT32 vertexSize;

	ID3D11RenderTargetView** renderTargetView;
	ID3D11ShaderResourceView** shaderResourceView;

	ID3D11ShaderResourceView ** clearRV;

	struct Vertex
	{

		float x, y, z;
		float U, V;


		Vertex()
		{

		}

		Vertex(float x, float y, float z, float U, float V)
		{
			this->x = x;
			this->y = y;
			this->z = z;

			this->U = U;
			this->V = V;
		}
	};

	ID3D11PixelShader * deferredPixelShader;
public:
	DeferredRendering(ID3D11Device* device, int _screenWidth, int _screenHeight);
	~DeferredRendering();


	void FirstPass(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView);
	void Render(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* gBackbufferRTV);
};
#endif