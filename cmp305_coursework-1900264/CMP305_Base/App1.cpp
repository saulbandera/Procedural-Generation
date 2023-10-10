#include "App1.h"

App1::App1()
{
	terrain = nullptr;
	water = nullptr;
	sun = nullptr;
	aboveThresholdBrightness = nullptr;
	downsampledTexture = nullptr;
	mainScene = nullptr;
	bloomShader = nullptr;
	thresholdCheckShader = nullptr;
	horizontalBlurShader = nullptr;
	verticalBlurShader = nullptr;
	orthoMesh = nullptr;
	renderNormals = 0;
}



///////////////////////////           [ INIT  ]           /////////////////////////// 
void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create scene objects
	water = new Water(hwnd, renderer, textureMgr);
	sun = new Sun(hwnd, renderer, textureMgr);
	terrain = new Terrain(hwnd,renderer, textureMgr, 600);

	skyBox = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), true, 100);
	skyboxShader = new TextureShader(renderer->getDevice(), hwnd); 
	textureMgr->loadTexture(L"sky", L"res/sky.png");


	// bloom related shaders initialisation 
	thresholdCheckShader = new ThresholdCheckShader(renderer->getDevice(), hwnd);
	thresholdCheckShader->setBrightnessThreshold(0.01);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0.0f, 0.0f);

	downsampleAmount = 7;
	int downsampledWidth = sWidth / downsampleAmount;
	int downsampledHeight = sHeight / downsampleAmount;

	// render textures used for post processing 
	mainScene = new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	sunTarget = new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	aboveThresholdBrightness = new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	downsampledTexture = new RenderTexture(renderer->getDevice(), downsampledWidth, downsampledHeight, 0.1f, 100.0f);


    // directional light definition 
	light = new Light;
	light->setAmbientColour(0.35f, 0.35f, 0.35f, 1.0f);
	light->setDiffuseColour(1.f, 1.f, 1.f, 1.0f);
	light->setPosition(50.0f, 70.0f, 130.0f);
	light->setDirection(0.0f, -1.0f, -0.7f);
	light->setSpecularColour(0.6f, 0.6f, 0.6f, 1.0f);
	light->setSpecularPower(200.0f);

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (terrain)
	{
		delete terrain;
		terrain = 0;
	}	

	if (water) {
		delete water;
	}

	if (sun) {
		delete sun;
	}

	if (skyBox) {
		delete skyBox;
	}

	if (skyboxShader) {
		delete skyboxShader;
	}

	if (light) {
		delete light;
	}

	if (mainScene) {
		delete mainScene;
	}
	if (sunTarget) {
		delete sunTarget;
	}

	if (aboveThresholdBrightness) {
		delete aboveThresholdBrightness;
	}

	if (downsampledTexture) {
		delete downsampledTexture;
	}

	if (thresholdCheckShader) {
		delete thresholdCheckShader;
	}

	if (bloomShader) {
		delete bloomShader;
	}

	if (horizontalBlurShader) {
		delete horizontalBlurShader;
	}

	if (verticalBlurShader) {
		delete verticalBlurShader;
	}

}

///////////////////////////           [ FRAME  ]           /////////////////////////// 
bool App1::frame()
{
	bool result;
	time.frame();

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}



///////////////////////////           [ RENDER PASS ]           ///////////////////////////                     
void App1::renderMainPass()
{
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();


	// Set the render target to be the main scene render texture
	mainScene->setRenderTarget(renderer->getDeviceContext());
	mainScene->clearRenderTarget(renderer->getDeviceContext(), 0.07f, 0.07f, 0.09f, 1.0f);

	//setting the zbuffer to false to render the skybox 
	renderer->setZBuffer(false);
	skyBox->sendData(renderer->getDeviceContext());
	skyboxShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixRotationY(10) * XMMatrixScaling(10, 10, 10) * XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sky"), renderNormals);
	skyboxShader->render(renderer->getDeviceContext(), skyBox->getIndexCount());
	renderer->setZBuffer(true);

	// render the main scene objects to texture 
	if (renderWater) {
		water->render(viewMatrix, light, timeVar, camera->getPosition(), dirBrightness, renderNormals);
	}

	if (renderTerrain) {
		terrain->render(viewMatrix, light, camera->getPosition(), dirBrightness, renderNormals);
	}

	sun->render(viewMatrix, light->getPosition(), camera->getPosition(), renderNormals);

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ SUN PASS ]           ///////////////////////////                     
void App1::renderSunPass()
{
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Set the render target to be the sun render texture
	sunTarget->setRenderTarget(renderer->getDeviceContext());
	sunTarget->clearRenderTarget(renderer->getDeviceContext(), 0.f, 0.f, 0.f, 1.0f);

	
	sun->render(viewMatrix, light->getPosition(), camera->getPosition(), renderNormals);


	// render terrain using black texture so that it blocks the bloom effect if it is covering the sun
	if (renderTerrain) {
		terrain->renderInBlack(viewMatrix, camera->getPosition(), false);
	}

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ BLOOM PASS ]           ///////////////////////////                     
void App1::bloomPass()
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	// Set the render target to be the threshold texture 
	aboveThresholdBrightness->setRenderTarget(renderer->getDeviceContext());
	aboveThresholdBrightness->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Send the sun render texture and check that each pixel is above a certain level of brightness, if it is then it gets rendered to new render texture
	orthoMesh->sendData(renderer->getDeviceContext());
	thresholdCheckShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sunTarget->getShaderResourceView(), XMFLOAT4(0,0,0,0));
	thresholdCheckShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();


	// Set the render target to be the downsampled render texture
	downsampledTexture->setRenderTarget(renderer->getDeviceContext());
	downsampledTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// the size of the downsampled render texture 
	XMFLOAT2 size = XMFLOAT2(downsampledTexture->getTextureWidth(), downsampledTexture->getTextureHeight());

	// ortho mesh is renderered with the horizontally blurred texture of bright areas 
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, aboveThresholdBrightness->getShaderResourceView(), size.x);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();


	// Set the render target back to the sun render texture
	sunTarget->setRenderTarget(renderer->getDeviceContext());
	sunTarget->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	// ortho mesh is rendered with the vertically blurred texture of the horizontally-blurred-downsampled texture  
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downsampledTexture->getShaderResourceView(), size.y);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ FINAL PASS ]           ///////////////////////////                     
void App1::finalPass()
{
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	if (!renderer->getWireframeState()) {
		// render the ortho mesh textured with the combined full scene and bloomed sun texture 
		orthoMesh->sendData(renderer->getDeviceContext());
		bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, mainScene->getShaderResourceView(), sunTarget->getShaderResourceView());
		bloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	}
	else {
		// if wireframe is on dont render ortho mesh as it covers the screen 
		if (renderWater) {
			water->render(viewMatrix, light, timeVar, camera->getPosition(), dirBrightness, renderNormals);
		}
		if (renderTerrain) {
			terrain->render(viewMatrix, light, camera->getPosition(), dirBrightness, renderNormals);
		}
		sun->render(viewMatrix, light->getPosition(), camera->getPosition(), renderNormals);
	}

}




///////////////////////////           [ RENDER ]           /////////////////////////// 
bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// advance the time, used for wave movement
	timeVar += time.getTime();

	renderMainPass(); // render main scene to texture
	renderSunPass(); // render sun to texture
	bloomPass(); // apply bloom to sun render texture 
	finalPass(); // render the final scene with bloom

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();
	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Render normals", &renderNormals);
	ImGui::Checkbox("Render terrain", &renderTerrain);
	ImGui::Checkbox("Render water", &renderWater);

	// terrain settings
	terrain->GUI();
	// water settings
	water->GUI();
	// sun settings
	sun->GUI(light, dirBrightness);
	// camera settings
	camera->ui();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

