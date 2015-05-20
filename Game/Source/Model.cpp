#include"Model.h"

Model::Model()
{
	DirectX::XMStoreFloat4x4(&objMatrix, DirectX::XMMatrixIdentity());
	normalShaderResource = nullptr;
	meshVertexBuff = nullptr;
	meshIndexBuff = nullptr;
	pixelShaderMaterialCB = nullptr;
	indexCount = 0;
}

Model::~Model()
{

}

bool Model::Initialize(std::wstring _modelName, ID3D11Device* _device, std::vector<XMFLOAT3> *collidableGeometryPositions, std::vector<DWORD> *collidableGeometryIndices)
{
	bool result = true;

	result = LoadObj(_modelName, _device, collidableGeometryPositions, collidableGeometryIndices);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}


	return true;
}

bool Model::Initialize(std::wstring _modelName, ID3D11Device* _device, std::vector<XMFLOAT3> *collidableGeometryPositions, std::vector<DWORD> *collidableGeometryIndices, bool pickable)
{
	bool result = true;

	result = LoadObj(_modelName, _device, collidableGeometryPositions, collidableGeometryIndices, pickable);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}


	return true;
}

bool Model::Initialize(std::wstring _modelName, ID3D11Device* _device)
{
	bool result = true;

	result = LoadObj(_modelName, _device);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}


	return true;
}

void Model::Shutdown()
{

	// Release the index buffer.
	if (meshIndexBuff)
	{
		meshIndexBuff->Release();
		meshIndexBuff = 0;
	}

	// Release the vertex buffer.
	if (meshVertexBuff)
	{
		meshVertexBuff->Release();
		meshVertexBuff = 0;
	}

	for (size_t i = 0; i < modelMats.size(); i++)
	{
		if (modelMats.at(i).texture)
		{
			modelMats.at(i).texture->Release();
			modelMats.at(i).texture = 0;
		}
	}

	if (normalShaderResource)
	{
		normalShaderResource->Release();
		normalShaderResource = 0;
	}

	if (pixelShaderMaterialCB)
	{
		pixelShaderMaterialCB->Release();
		pixelShaderMaterialCB = 0;
	}
}

void Model::Update(Camera* camera)
{

}

void Model::Render(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilState * _depthState)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &meshVertexBuff, &vertexSize, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pixelShaderMaterialCB)
		_deviceContext->PSSetConstantBuffers(0, 1, &pixelShaderMaterialCB);

	int indexStart = 0;

	for (size_t n = 0; n < modelMats.size(); n++)
	{
		_deviceContext->PSSetShaderResources(0, 1, &modelMats.at(n).texture);

		_deviceContext->DrawIndexed(modelMats.at(n).indexAmount, indexStart, 0);
		indexStart += modelMats.at(n).indexAmount;
	}

	return;
}

void Model::Render(ID3D11DeviceContext* _deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &meshVertexBuff, &vertexSize, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pixelShaderMaterialCB)
		_deviceContext->PSSetConstantBuffers(0, 1, &pixelShaderMaterialCB);

	int indexStart = 0;

	for (size_t n = 0; n < modelMats.size(); n++)
	{
		_deviceContext->PSSetShaderResources(0, 1, &modelMats.at(n).texture);

		_deviceContext->DrawIndexed(modelMats.at(n).indexAmount, indexStart, 0);
		indexStart += modelMats.at(n).indexAmount;
	}

	return;
}

bool Model::LoadObj(std::wstring _filename, ID3D11Device* _device, std::vector<XMFLOAT3> *collidableGeometryPositions, std::vector<DWORD> *collidableGeometryIndices, bool pickable)
{
	HRESULT hr = 0;

	std::wifstream fileIn((_filename + L".obj").c_str());	//Open file

	std::wstring meshMatLib;					//String to hold our obj material library filename

	//Arrays to store our model's information
	std::vector<DWORD> indices;
	std::vector<DirectX::XMFLOAT3> vertPos;
	std::vector<DirectX::XMFLOAT3> vertNorm;
	std::vector<DirectX::XMFLOAT2> vertTexCoord;

	int meshSubsets = 0;
	std::vector<int> meshSubsetIndexStart;

	//Vertex definition indices
	std::vector<int> vertPosIndex;
	std::vector<int> vertNormIndex;
	std::vector<int> vertTCIndex;

	//Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNorm = false;

	int materialIndex = 0;

	//Material loading 
	std::vector<Material> materials;

	//Temp variables to store into vectors
	std::wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	wchar_t checkChar;		//The variable we will use to store one char from file at a time
	std::wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	//Check to see if the file was opened
	if (fileIn)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
			case '#':
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;
			case 'v':	//Get Vertex Descriptions
				checkChar = fileIn.get();
				if (checkChar == ' ')	//v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;	//Store the next three types

					vertPos.push_back(DirectX::XMFLOAT3(vx, vy, vz));
				}
				if (checkChar == 't')	//vt - vert tex coords
				{
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;		//Store next two types

					vertTexCoord.push_back(DirectX::XMFLOAT2(vtcu, vtcv));

					hasTexCoord = true;	//We know the model uses texture coords
				}

				if (checkChar == 'n')	//vn - vert normal
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;	//Store next three types

					vertNorm.push_back(DirectX::XMFLOAT3(vnx, vny, vnz));

					hasNorm = true;	//We know the model defines normals
				}
				break;

				//New group (Subset)
			case 'g':	//g - defines a group
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					meshSubsetIndexStart.push_back(vIndex);		//Start index for this subset
					meshSubsets++;
				}
				break;

				//Get Face Index
			case 'f':	//f - defines the faces
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					face = L"";
					std::wstring VertDef;	//Holds one vertex definition at a time
					triangleCount = 0;

					checkChar = fileIn.get();
					while (checkChar != '\n')
					{
						face += checkChar;			//Add the char to our face string
						checkChar = fileIn.get();	//Get the next Character
						if (checkChar == ' ')		//If its a space...
							triangleCount++;		//Increase our triangle count
					}

					//Check for space at the end of our face string
					if (face[face.length() - 1] == ' ')
						triangleCount--;	//Each space adds to our triangle count

					triangleCount -= 1;		//Ever vertex in the face AFTER the first two are new faces

					std::wstringstream ss(face);

					if (face.length() > 0)
					{
						int firstVIndex, lastVIndex;	//Holds the first and last vertice's index

						materialIndex += 3;
						for (int i = 0; i < 3; ++i)		//First three vertices (first triangle)
						{
							ss >> VertDef;	//Get vertex definition (vPos/vTexCoord/vNorm)

							std::wstring vertPart;
							int whichPart = 0;		//(vPos, vTexCoord, or vNorm)

							//Parse this string
							for (size_t j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')	//If there is no divider "/", add a char to our vertPart
									vertPart += VertDef[j];

								//If the current char is a divider "/", or its the last character in the string
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

									if (whichPart == 0)	//If vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1

										//Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}

									else if (whichPart == 1)	//If vTexCoord
									{
										if (vertPart != L"")	//Check to see if there even is a tex coord
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;	//subtract one since c++ arrays start with 0, and obj start with 1
										}
										else	//If there is no tex coord, make a default
											vertTCIndexTemp = 0;

										//If the cur. char is the second to last in the string, then
										//there must be no normal, so set a default normal
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2)	//If vNorm
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1
									}

									vertPart = L"";	//Get ready for next vertex part
									whichPart++;	//Move on to next vertex part					
								}
							}

							//Check to make sure there is at least one subset
							if (meshSubsets == 0)
							{
								meshSubsetIndexStart.push_back(vIndex);		//Start index for this subset
								meshSubsets++;
							}

							//Avoid duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								//Loop through all the vertices
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									//If the vertex position and texture coordinate in memory are the same
									//As the vertex position and texture coordinate we just now got out
									//of the obj file, we will set this faces vertex index to the vertex's
									//index value in memory. This makes sure we don't create duplicate vertices
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);		//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							//If this vertex is not already in our vertex arrays, put it there
							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;	//We created a new vertex
								indices.push_back(totalVerts - 1);	//Set index for this vertex
							}

							//If this is the very first vertex in the face, we need to
							//make sure the rest of the triangles use this vertex
							if (i == 0)
							{
								firstVIndex = indices[vIndex];	//The first vertex index of this FACE

							}

							//If this was the last vertex in the first triangle, we will make sure
							//the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if (i == 2)
							{
								lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE
							}
							vIndex++;	//Increment index count
						}

						meshTriangles++;	//One triangle down
					}
				}
				break;

			case 'm':	//mtllib - material library filename
				checkChar = fileIn.get();
				if (checkChar == 't')
				{
					checkChar = fileIn.get();
					if (checkChar == 'l')
					{
						checkChar = fileIn.get();
						if (checkChar == 'l')
						{
							checkChar = fileIn.get();
							if (checkChar == 'i')
							{
								checkChar = fileIn.get();
								if (checkChar == 'b')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										//Store the material libraries file name
										fileIn >> meshMatLib;
									}
								}
							}
						}
					}
				}
				break;
			
			case 'u':	//usemtl - which material to use
				checkChar = fileIn.get();
				if (checkChar == 's')
				{
					checkChar = fileIn.get();
					if (checkChar == 'e')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										if (materialIndex != 0)
										{
											modelMats.back().indexAmount = materialIndex; //.push_back(materialIndex);
											materialIndex = 0;
										}

										ModelMaterial temp;
										temp.matname = L"";	//Make sure this is cleared
										
										fileIn >> temp.matname; //Get next type (string)
										modelMats.push_back(temp);
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
	else	//If we could not open the file
	{
		//create message
		std::wstring message = L"Could not open: ";
		message += _filename;

		MessageBox(0, message.c_str(),	//display message
			L"Error", MB_OK);

		return false;
	}

	if (materialIndex != 0)
	{
		modelMats.back().indexAmount = materialIndex; //.push_back(materialIndex);
		materialIndex = 0;
	}

	meshSubsetIndexStart.push_back(vIndex); //There won't be another index start after our last subset, so set it here


#pragma region Load mtl file
	//Close the obj file, and open the mtl file
	fileIn.close();
	fileIn.open(meshMatLib.c_str());

	int vectorIndex = 0;
	//kdset - If our diffuse color was not set, we can use the ambient color (which is usually the same)
	//If the diffuse color WAS set, then we don't need to set our diffuse color to ambient
	bool kdset = false;

	if (fileIn)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
				//Check for comment
			case '#':
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;
				//Get the diffuse map (texture)
			case 'K':
				checkChar = fileIn.get();
				if (checkChar == 'd')	//Diffuse Color
				{
					checkChar = fileIn.get();	//remove space

					fileIn >> modelMats.at(vectorIndex).material.Kd.x;
					fileIn >> modelMats.at(vectorIndex).material.Kd.y;
					fileIn >> modelMats.at(vectorIndex).material.Kd.z;
				}

				//Ambient Color (We'll store it in diffuse if there isn't a diffuse already)
				if (checkChar == 'a')
				{
					checkChar = fileIn.get();	//remove space
					if (!kdset)
					{
						fileIn >> modelMats.at(vectorIndex).material.Ka.x;
						fileIn >> modelMats.at(vectorIndex).material.Ka.y;
						fileIn >> modelMats.at(vectorIndex).material.Ka.z;
					}
				}
				break;

			case 'T':
				checkChar = fileIn.get();
				if (checkChar == 'r')
				{
					checkChar = fileIn.get();	//remove space
					float Transparency;
					fileIn >> Transparency;

					modelMats.at(vectorIndex).material.Tr = Transparency;

				}
				break;

			case 'd':
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					float Transparency;
					fileIn >> Transparency;

					//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
					Transparency = 1.0f - Transparency;

					modelMats.at(vectorIndex).material.d = Transparency;
				}
				break;

			case 'm':
				checkChar = fileIn.get();
				if (checkChar == 'a')
				{
					checkChar = fileIn.get();
					if (checkChar == 'p')
					{
						checkChar = fileIn.get();
						if (checkChar == '_')
						{
							//map_Kd - Diffuse map
							checkChar = fileIn.get();
							if (checkChar == 'K')
							{
								checkChar = fileIn.get();
								if (checkChar == 'd')
								{
									std::wstring fileNamePath;

									fileIn.get();	//Remove whitespace between map_Kd and file

									//Get the file path - We read the pathname char by char since
									//pathnames can sometimes contain spaces, so we will read until
									//we find the file extension
									bool texFilePathEnd = false;
									while (!texFilePathEnd)
									{
										checkChar = fileIn.get();

										fileNamePath += checkChar;

										if (checkChar == '.')
										{
											for (int i = 0; i < 3; ++i)
												fileNamePath += fileIn.get();

											texFilePathEnd = true;
										}
									}

									hr = DirectX::CreateDDSTextureFromFile(_device, fileNamePath.c_str(), NULL, &modelMats.at(vectorIndex).texture);
								}
							}

						}
					}
				}
				break;

			case 'n':	//newmtl - Declare new material
				checkChar = fileIn.get();
				if (checkChar == 'e')
				{
					checkChar = fileIn.get();
					if (checkChar == 'w')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										//New material, set its defaults
										Material tempMat;
										std::wstring tempName;
										fileIn >> tempName;

										for (size_t i = 0; i < modelMats.size(); i++)
										{
											if (modelMats.at(i).matname == tempName)
											{
												vectorIndex = i;
											}
										}
									}
								}
							}
						}
					}
				}
				break;


			default:
				break;
			}
		}
	}
	else
	{
		return false;
	}

#pragma endregion

	for (size_t i = 0; i < modelMats.size(); i++)
	{
		for (size_t n = i; n < modelMats.size(); n++)
		{
			if (modelMats.at(i).matname == modelMats.at(n).matname)
			{
				modelMats.at(i).material = modelMats.at(n).material;
				modelMats.at(i).texture = modelMats.at(n).texture;
			}
		}
	}

	//sometimes "g" is defined at the very top of the file, then again before the first group of faces.
	//This makes sure the first subset does not conatain "0" indices.
	if (meshSubsetIndexStart[1] == 0)
	{
		meshSubsetIndexStart.erase(meshSubsetIndexStart.begin() + 1);
		meshSubsets--;
	}

	//Make sure we have a default for the tex coord and normal
	//if one or both are not specified
	if (!hasNorm)
	{
		vertNorm.push_back(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	if (!hasTexCoord)
	{
		vertTexCoord.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
	}

	//Close the obj file, and open the mtl file
	fileIn.close();

	std::wstring lastStringRead;

	std::vector<Vertex> vertices;
	Vertex tempVert;

	//Create our vertices using the information we got 
	//from the file and store them in a vector
	for (int j = 0; j < totalVerts; ++j)
	{
		tempVert.position = DirectX::XMFLOAT4(vertPos[vertPosIndex[j]].x, vertPos[vertPosIndex[j]].y, vertPos[vertPosIndex[j]].z, 1.0f);
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texture = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
	}


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD)* meshTriangles * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	_device->CreateBuffer(&indexBufferDesc, &iinitData, &meshIndexBuff);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Model::Vertex) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &meshVertexBuff);



	indexCount = meshSubsetIndexStart[meshSubsetIndexStart.size() - 1];


	if (collidableGeometryIndices != nullptr && collidableGeometryPositions != nullptr)
	{
		int vertexOffset = collidableGeometryPositions->size();	// Vertex offset

		// Temp arrays because we need to store the geometry in world space
		XMVECTOR tempVertexPosVec;
		XMFLOAT3 tempVertF3;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			tempVertexPosVec = XMLoadFloat4(&vertices[i].position);
			tempVertexPosVec = XMVector3TransformCoord(tempVertexPosVec, XMLoadFloat4x4(&objMatrix));
			XMStoreFloat3(&tempVertF3, tempVertexPosVec);
			collidableGeometryPositions->push_back(tempVertF3);
			if (pickable)
			{
				pickingPoints.push_back(tempVertF3);
			}
		}

		for (size_t i = 0; i < indices.size(); i++)
		{
			collidableGeometryIndices->push_back(indices[i] + vertexOffset);
			if (pickable)
			{
				pickingIndices.push_back(indices[i]);
			}
		}
	}

	return true;
}


bool Model::CreateShaders(ID3D11Device* _device)
{
	D3D11_BUFFER_DESC materialDesc;
	D3D11_SUBRESOURCE_DATA materialSubResource;

	memset(&materialDesc, 0, sizeof(materialDesc));

	materialSubResource.pSysMem = &modelMats.front().material;
	materialSubResource.SysMemPitch = 0;
	materialSubResource.SysMemSlicePitch = 0;


	materialDesc.ByteWidth = sizeof(Material);
	materialDesc.Usage = D3D11_USAGE_DEFAULT;
	materialDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialDesc.MiscFlags = 0;
	materialDesc.StructureByteStride = 0;


	HRESULT hr = _device->CreateBuffer(&materialDesc, &materialSubResource, &pixelShaderMaterialCB);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

int Model::GetIndexCount()
{
	return indexCount;
}

DirectX::XMMATRIX Model::GetObjMatrix()
{
	return DirectX::XMLoadFloat4x4(&objMatrix);
}

void Model::SetObjMatrix(const DirectX::XMMATRIX& _newMatrix)
{
	DirectX::XMStoreFloat4x4(&objMatrix, _newMatrix);
}

std::vector<XMFLOAT3>* Model::GetPickingPoints()
{
	return &pickingPoints;
}

std::vector<DWORD>* Model::GetPickingIndicies()
{

	return &pickingIndices;
}