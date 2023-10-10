#pragma once
#include "DXF.h"
#include "WaveShader.h"
#include <iostream>
#include <Timer.h>
class Water
{

public:
	Water(HWND hwnd, D3D* renderer, TextureManager* textureMgr);
	~Water();

	void render(XMMATRIX &camview, Light* light, float time, XMFLOAT3 camPos, float dirBrightness, bool normals);
	void GUI();
	

private:
	WaveShader* wavesShader; // shader used to render the water
	PlaneMesh* waveMesh;     // plane mesh to be manipulated by gerstner function 
	D3D* renderer;
	TextureManager* textureMgr;

	// variables to change water appearance 
	float wavelength; // amount of waves
	float speed;      // speed at which the waves move
	float amplitude;  // height of the waves
	float steepness;  // steepness of the waves
};

