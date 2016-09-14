#ifndef MATERIAL_H
#define MATERIAL_H

#include <DirectXMath.h>

struct Material
{
	float Ns; //Specular Power
	float Ni; //Optical Density, 0-10, jobbar med transparanta objekt
	float d; // Transparacy
	float Tr; // Transparacy igen anv�nds utav ett annat filformat
	DirectX::XMFLOAT3 Tf; //Transmission Filter, filtrerar ljus som passerar igenom transparanta objekt
	int illum; // Ljuss�ttnings tjofr�s , 0-10
	DirectX::XMFLOAT3 Ka; //Ambient Color, detta �r f�rgen p� ett objekt n�r inget ljus �r p� den
	DirectX::XMFLOAT3 Kd; //Diffuse map, kolla denna om objekt inte har texturer
	DirectX::XMFLOAT3 Ks; //Specular Color �r f�rgen som objektet f�r n�r ljus reflekteras emot den
	DirectX::XMFLOAT3 Ke;//Emissive color is the color of a surface when it "lights up". For example a lightbulb.
};

#endif