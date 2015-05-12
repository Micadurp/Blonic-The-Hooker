#ifndef LIGHTCLASS_H
#define LIGHTCLASS_H

#include <DirectXMath.h>

using namespace DirectX;

class LightClass
{
	private:
		XMFLOAT4 position;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		XMFLOAT3 attenuation;

		float intensity;
		float range;

	public:
		LightClass();
		~LightClass();

		bool Initialize(const XMFLOAT4 &_pos, const XMFLOAT4 &_amb, const XMFLOAT4 &_diff, const XMFLOAT3 &_att = { 0.0f, 0.0f, 0.3f }, int _intens = 1000, int _range = 100);

		void SetPosition(float _x, float _y, float _z);
		void SetAmbient(float _r, float _g, float _b, float _a = 1.0f);
		void SetDiffuse(float _r, float _g, float _b, float _a = 1.0f);

		void SetAttenuation(float _x, float _y, float _z);

		void SetRange(float _range);
		void SetIntensity(float _intensity);

		float GetRange() const;
		float GetIntensity() const;

		XMFLOAT4 GetPosition() const;
		XMFLOAT4 GetAmbient() const;
		XMFLOAT4 GetDiffuse() const;
		XMFLOAT3 GetAttenuation() const;

		LightClass* GetLight();
};

#endif