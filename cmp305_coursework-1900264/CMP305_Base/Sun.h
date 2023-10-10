#pragma once
#include "DXF.h"
#include "TextureShader.h"
#include <iostream>
#include <Timer.h>
#include "imGUI\imgui.h"
class Sun
{

public:
	Sun(HWND hwnd, D3D* renderer, TextureManager* textureMgr);
	~Sun();
	void render(XMMATRIX& camview, XMFLOAT3 lightPos, XMFLOAT3 camPos , bool normals);
	void GUI(Light* light, float &dirBrightness);


private:
	
	TextureShader* sunShader;
	SphereMesh* sunMesh;
	D3D* renderer;
	TextureManager* textureMgr;
};

