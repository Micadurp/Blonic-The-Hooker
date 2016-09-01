#ifndef MATERIAL_H
#define MATERIAL_H

#include <DirectXMath.h>

struct Material
{
	float Ns; //Specular Power
	float Ni; //Optical Density, 0-10, jobbar med transparanta objekt
	float d; // Transparacy
	float Tr; // Transparacy igen används utav ett annat filformat
	DirectX::XMFLOAT3 Tf; //Transmission Filter, filtrerar ljus som passerar igenom transparanta objekt
	int illum; // Ljussättnings tjofräs , 0-10
	DirectX::XMFLOAT3 Ka; //Ambient Color, detta är färgen på ett objekt när inget ljus är på den
	DirectX::XMFLOAT3 Kd; //Diffuse map, kolla denna om objekt inte har texturer
	DirectX::XMFLOAT3 Ks; //Specular Color är färgen som objektet får när ljus reflekteras emot den
	DirectX::XMFLOAT3 Ke;//Emissive color is the color of a surface when it "lights up". For example a lightbulb.
};

#endif