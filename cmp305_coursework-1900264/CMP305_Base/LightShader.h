#pragma once
#include "DXF.h"
using namespace std;
using namespace DirectX;

// Shader used to render the terrain, performs the lighting calculations and textures 
// the terrain. 

class LightShader : public BaseShader
{
private:
	struct LightBufferType {
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 position;
		// following xmfloat is used to store the specular power of the light
		// and two bools, one to determine if the terrain is a volcano and 
		// another to determine if normals should be rendered. 
		XMFLOAT4 specularPower_isVolcano_normals;
		XMFLOAT4 specular;
		XMFLOAT4 direction;
	};


	struct CameraBufferType
	{
		XMFLOAT3 position;
		float padding;
	};
	

public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture[3], Light* light, float dirBrightness, bool normals, XMFLOAT3 camPos, bool volcano);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;

};

