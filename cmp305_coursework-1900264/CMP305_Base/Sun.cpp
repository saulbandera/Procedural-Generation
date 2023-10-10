#include "Sun.h"

Sun::Sun(HWND hwnd, D3D* rendr, TextureManager* texMgr)
{
	renderer = rendr;
	textureMgr = texMgr;

	textureMgr->loadTexture(L"yellow", L"res/yellow.jpg");
	sunMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), false, 100);
	sunShader = new TextureShader(renderer->getDevice(), hwnd);
}

Sun::~Sun()
{
	if (sunShader) {
		delete sunShader;
	}
	if (sunMesh) {
		delete sunMesh;
	}
}
///////////////////////////           [ RENDER ]           /////////////////////////// 
void Sun::render(XMMATRIX& camview, XMFLOAT3 lightPos, XMFLOAT3 camPos, bool normals)
{
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = renderer->getWorldMatrix();
	// position of the sun is set to that of the light
	worldMatrix = XMMatrixTranslation(lightPos.x / 20.f, lightPos.y / 20.f, lightPos.z / 20.f); 
	XMMATRIX scaleMatrix = XMMatrixScaling(20.f, 20.f, 20.f); // scale of the sphere
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	sunMesh->sendData(renderer->getDeviceContext());
	sunShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, renderer->getProjectionMatrix(), textureMgr->getTexture(L"yellow"), normals);
	sunShader->render(renderer->getDeviceContext(), sunMesh->getIndexCount());
}

///////////////////////////           [ USER INTERFACE ]           /////////////////////////// 
void Sun::GUI(Light* light, float &dirBrightness)
{
	// directional light settings
	float dirDiffuse[4] = { light->getDiffuseColour().x, light->getDiffuseColour().y, light->getDiffuseColour().z ,light->getDiffuseColour().w };
	float dirAmbient[4] = { light->getAmbientColour().x, light->getAmbientColour().y, light->getAmbientColour().z ,light->getAmbientColour().w };
	float dirSpecular[4] = { light->getSpecularColour().x, light->getSpecularColour().y, light->getSpecularColour().z ,light->getSpecularColour().w };
	float specularValue = light->getSpecularPower();

	// User interface settings for the sun 
	if (ImGui::CollapsingHeader("Sun Settings")) { 

		if (ImGui::ColorEdit3(" Diffuse colour ", (float*)&dirDiffuse)) {
			light->setDiffuseColour(dirDiffuse[0], dirDiffuse[1], dirDiffuse[2], dirDiffuse[3]);
		}
		if (ImGui::ColorEdit3(" Ambient colour ", (float*)&dirAmbient)) {
			light->setAmbientColour(dirAmbient[0], dirAmbient[1], dirAmbient[2], dirAmbient[3]);
		}
		if (ImGui::ColorEdit3(" Specular colour ", (float*)&dirSpecular)) {
			light->setSpecularColour(dirSpecular[0], dirSpecular[1], dirSpecular[2], dirSpecular[3]);
		}
		if (ImGui::SliderFloat(" Specular power", &specularValue, 0, 200)) {
			light->setSpecularPower(specularValue);
		}

		ImGui::SliderFloat(" Brightness Value ", (float*)&dirBrightness, 0, 10);

		if (ImGui::Button(" Sun Default Values", ImVec2(170, 20))) {
			light->setAmbientColour(0.35f, 0.35f, 0.35f, 1.0f);
			light->setDiffuseColour(1.f, 1.f, 1.f, 1.0f);
			light->setSpecularColour(0.6f, 0.6f, 0.6f, 1.0f);
			light->setSpecularPower(200.0f);
			dirBrightness = 1;
		}
	}
}
