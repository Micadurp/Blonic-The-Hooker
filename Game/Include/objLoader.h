#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "DDSTextureLoader.h"

#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "Structs\ModelMaterial.h"
#include "Structs\Vertex.h"



class ObjLoader
{
private:

public:
	ObjLoader();
	virtual ~ObjLoader();
	static bool LoadObj(
		std::wstring _filename, 
		ID3D11Device* _device,
		ID3D11Buffer*& meshVertexBuff,
		ID3D11Buffer*& meshIndexBuff,
		int& indexCount,
		std::vector<DirectX::XMFLOAT3>& pickingPoints,
		std::vector<DWORD>& pickingIndices,
		DirectX::XMFLOAT4X4& objMatrix,
		std::vector<ModelMaterial>& modelMats,
		std::vector<DirectX::XMFLOAT3>* collidableGeometryPositions = nullptr, 
		std::vector<DWORD>* collidableGeometryIndices = nullptr, 
		bool pickable = false
	);

};
#endif