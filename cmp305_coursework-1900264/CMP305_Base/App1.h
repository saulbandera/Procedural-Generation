// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "Terrain.h"
#include "Water.h"
#include "Sun.h"
#include "BloomShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "ThresholdCheckShader.h"
#include "RenderTexture.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	bool frame();

protected:

	void renderMainPass();
	void renderSunPass();
	void bloomPass();
	void finalPass();
	bool render();
	void gui();	     

private:


	// Scene objects
	Water* water;
	Sun* sun;
	Terrain* terrain;
	SphereMesh* skyBox;
	TextureShader* skyboxShader; // shader used to render skybox
	Light* light;


	// bloom effect related render textures 
	RenderTexture* sunTarget;
	RenderTexture* aboveThresholdBrightness;
	RenderTexture* downsampledTexture;
	RenderTexture* mainScene;


	// bloom effect related shaders
	BloomShader* bloomShader;
	ThresholdCheckShader* thresholdCheckShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;
	int downsampleAmount;
	// 2D orthographic mesh used for post processing
	OrthoMesh* orthoMesh;
	

	Timer time;
	float timeVar;
	bool renderNormals;
	bool renderTerrain = true;
	bool renderWater = true;
	float dirBrightness = 1;
};

#endif