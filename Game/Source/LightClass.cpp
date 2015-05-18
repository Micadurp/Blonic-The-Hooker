#include "LightClass.h"

LightClass::LightClass()
{
}

LightClass::~LightClass()
{
}

bool LightClass::Initialize(const XMFLOAT4 &_pos, const XMFLOAT4 &_amb, const XMFLOAT4 &_diff, const XMFLOAT3 &_att, int _intens, int _range)
{
	position = _pos;
	ambient = _amb;
	diffuse = _diff;

	attenuation = _att;

	intensity = (float) _intens;
	range = (float) _range;

	return true;
}

void LightClass::SetPosition(float _x, float _y, float _z)
{
	position = { _x, _y, _z, 1.0f };
}

void LightClass::SetAmbient(float _r, float _g, float _b, float _a)
{
	ambient = { _r, _g, _b, _a };
}

void LightClass::SetDiffuse(float _r, float _g, float _b, float _a)
{
	diffuse = { _r, _g, _b, _a };
}

void LightClass::SetAttenuation(float _x, float _y, float _z)
{
	attenuation = { _x, _y, _z };
}

void LightClass::SetRange(float _range)
{
	range = _range;
}

void LightClass::SetIntensity(float _intens)
{
	intensity = _intens;
}

float LightClass::GetRange() const
{
	return range;
}

float LightClass::GetIntensity() const
{
	return intensity;
}

XMFLOAT4 LightClass::GetPosition() const
{
	return position;
}

XMFLOAT4 LightClass::GetAmbient() const
{
	return ambient;
}

XMFLOAT4 LightClass::GetDiffuse() const
{
	return diffuse;
}

XMFLOAT3 LightClass::GetAttenuation() const
{
	return attenuation;
}

LightClass* LightClass::GetLight()
{
	return this;
}