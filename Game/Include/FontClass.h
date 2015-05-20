#ifndef FONTCLASS_H
#define FONTCLASS_H


#pragma comment (lib, "d3d11.lib")

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "DDSTextureLoader.h"

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};
	
	struct VertexType
	{
		XMFLOAT4 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	FontType* m_Font;
	ID3D11ShaderResourceView* m_texture;

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device* _device, char* _fontFileName, WCHAR* _texName);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void* _vertices, const char* _sentence, float _drawX, float _drawY);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* _device, WCHAR* _texName);
	void ReleaseTexture();
};

#endif