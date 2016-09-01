#ifndef MODELMATERIAL_H
#define MODELMATERIAL_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Material.h"

struct ModelMaterial
{
	std::wstring matname = L"";
	ID3D11ShaderResourceView* texture = nullptr;
	DWORD indexAmount;
	Material material;
};

#endif