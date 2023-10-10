#pragma once
#include "DXF.h"
#include "BaseShader.h"
// Basic shader used to render a mesh with a texture. vertex shader transfroms the mesh into
// world space and the pixel shader applies the texture. 

class TextureShader : public BaseShader
{

private:
	struct NormalsBufferType {
		XMFLOAT4 normals;
	};

public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture , bool normals);



private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* normalsBuffer;

};
