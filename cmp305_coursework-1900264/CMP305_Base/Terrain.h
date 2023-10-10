#pragma once
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include "PlaneMesh.h"
#include "imGUI\imgui.h"
#include "DXF.h"
#include "LightShader.h"
#include "Perlin.h"
#include "TextureShader.h"


class Terrain :
	public PlaneMesh {
public:
	Terrain(HWND hwnd, D3D* renderer,  TextureManager* textureMgr, int resolution = 100);
	~Terrain();

	void render(XMMATRIX& camview, Light* light, XMFLOAT3 camPos, float dirBrightness, bool normals);
	void renderInBlack(XMMATRIX& camview, XMFLOAT3 camPos, bool normals);
	void GUI();

private:

	void Resize(int newResolution);
	void Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void HydraulicErosion(float x, float y, VertexType*);
	void BlurTerrain(VertexType*);
	void ApplyErosion(VertexType*);
	void GenerateVolcanoes(VertexType*);

	void CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices);
	float DistanceToCenterGradient(int posX, int posZ, float radius, XMINT2 center, float steepness);

	void GenerateHeightMap();
	const float m_UVscale = 10.0f;			// Tile the UV map 10 times across the plane
	const float terrainSize = 200.0f;		// What is the width and height of our terrain
	float* heightMap;

	D3D* renderer;
	TextureManager* textureMgr;
	ID3D11ShaderResourceView* textures[3];  // textures used for terrain

	// radii for radial gradients 
	float islandRadius1, islandRadius2, islandRadius3, islandRadius4, islandRadius5, islandRadius6;

	Perlin perlin; // used to obtain 2D perlin noise


	LightShader* shader;          // render the terrain and perform lighting calculations 
	TextureShader* textureShader; // used to render terrain in black when performing bloom




	// values for erosion 
	int dropplets = 3500; // amount of dropplets simulated
	float depositionRate = 0.149f; // amount of material deposited 
	float erosionRate = 9.8f; // amout of material eroded
	float speed = 1.1948f; // speed of dropplets
	float friction = 0.399f; // friction of terrain 
	const int maxIterations = 80; // number of times each dropplet is simulated 
	const int blurTimes = 10; // amount of times terrain is blurred
	const float iterationScale = 0.07f; // used to increase the erosion of a dropplet as it is simulated
	const float deltaMultiplier = 1.133f; // multiplier for the amount of deformation caused by the dropplets

	bool volcanoTerrain = false;
	bool archipelagoTerrain = false;
	bool islandTerrain = true; 

	// frequencies and amplitudes used for fractional brownian motion
	float frequencyOct1 = 0.02f;
	float frequencyOct2 = 0.03f;
	float frequencyOct3 = 0.10f;
	float amplitudeOct1 = 2.0f;
	float amplitudeOct2 = 1.0f;
	float amplitudeOct3 = 0.5f;

};
