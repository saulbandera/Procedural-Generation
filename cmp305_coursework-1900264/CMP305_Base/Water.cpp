#include "Water.h"

Water::Water(HWND hwnd, D3D* rendr, TextureManager* texMgr)
{
	renderer = rendr;
	textureMgr = texMgr;

	textureMgr->loadTexture(L"waves", L"res/bluesea1.jpg");
	waveMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 600);
	wavesShader = new WaveShader(renderer->getDevice(), hwnd);

	// initial values for waves
	speed = 3.f; 
	amplitude = 0.3f; 
	steepness = 0.9f; 
	wavelength = 20.0f;
}

Water::~Water()
{
	if (wavesShader) {
		delete wavesShader;
	}
	if (waveMesh) {
		delete waveMesh;
	}
}
///////////////////////////           [ RENDER ]           /////////////////////////// 
void Water::render(XMMATRIX& camview, Light* light, float time, XMFLOAT3 camPos, float dirBrightness, bool normals)
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50, -0.5, -50); // position of the water plane

	// render the wave mesh plane using the waves shader that is responsible for manipulating the vertices of the plane 
	waveMesh->sendData(renderer->getDeviceContext());
	wavesShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, renderer->getProjectionMatrix(), textureMgr->getTexture(L"waves"), light, time, camPos, speed, amplitude, wavelength, steepness, dirBrightness, normals);
	wavesShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());
}


///////////////////////////           [ USER INTERFACE ]           /////////////////////////// 
void Water::GUI()
{
	// User interface settings for the wave 
	if (ImGui::CollapsingHeader("Water Settings")) {
		ImGui::SliderFloat(" Speed", &speed, 0.0f, 100.0f);
		ImGui::SliderFloat(" Amplitude", &amplitude, 0.01f, 3.0f);
		ImGui::SliderFloat(" Steepness", &steepness, 0.0f, 1.0f);
		ImGui::SliderFloat(" Wavelength", &wavelength, 1.0f, 100.f);


		if (ImGui::Button(" Water Default Values", ImVec2(170, 20))) {
			speed = 3.f;
			amplitude = 0.3f;
			steepness = 0.9f;
			wavelength = 20.0f;
		}
	}		

}
