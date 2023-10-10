#pragma once
#include "DXF.h"
#include "BaseShader.h"
// Shader used to manipulate the vertices of a plane with a gerstner function to produce
// a water-like surface. It also performs the lighting calculations for the water. 

class WaveShader : public BaseShader
{
private:
	struct LightBufferType {
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 position;
		XMFLOAT4 specularPower;
		XMFLOAT4 specular;
		XMFLOAT4 direction;
	};

	struct TimeBufferType {
		float time; 
		float speed;
		float amplitude;
		float steepness;
		// the following xmflaot contains the value of Pi and the wavelength 
		XMFLOAT2A PI_and_Wavelength;
	};

	struct CameraBufferType
	{
		XMFLOAT3 position;
		float padding;
	};



public:
	WaveShader(ID3D11Device* device, HWND hwnd);
	~WaveShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float time, XMFLOAT3 camPos, float speed, float amplitude, float wavelength, float steepness, float dirBrightness, bool normals);



private: 
	void initShader(const wchar_t* cs, const wchar_t* ps);



	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
};

