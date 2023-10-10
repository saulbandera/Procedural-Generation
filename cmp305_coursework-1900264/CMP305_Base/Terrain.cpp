#include "Terrain.h"
#include <iostream>

Terrain::Terrain(HWND hwnd,  D3D* rendr, TextureManager* texMgr, int lresolution) :
	PlaneMesh(rendr->getDevice(), rendr->getDeviceContext(), lresolution)
{
	renderer = rendr;
	textureMgr = texMgr;
	srand(time(NULL)); 

	//radial gradient radii (size of the islands)
	islandRadius1 = 20;
	islandRadius2 = 16;
	islandRadius3 = 13;
	islandRadius4 = 20;
	islandRadius5 = 16;
	islandRadius6 = 13;

	// generate the terrain
	Resize(resolution);
	heightMap = new float[resolution * resolution];
	GenerateHeightMap();
	Regenerate(renderer->getDevice(), renderer->getDeviceContext());

	// shaders used for terrain rendering
	textureShader = new TextureShader(renderer->getDevice(), hwnd); // used for terrain rendering in black (used for bloom post processing effect)
	shader = new LightShader(renderer->getDevice(), hwnd);          // used for terrain rendering with lighting

	// Load textures
	textureMgr->loadTexture(L"sand", L"res/sand1.jpg");
	textureMgr->loadTexture(L"grass", L"res/green.jpg");
	textureMgr->loadTexture(L"rock", L"res/grey.jpg");
	textureMgr->loadTexture(L"black", L"res/black.jpg");

	textures[0] = textureMgr->getTexture(L"sand");
	textures[1] = textureMgr->getTexture(L"grass");
	textures[2] = textureMgr->getTexture(L"rock");
}


Terrain::~Terrain() {

	delete[] heightMap;
	heightMap = 0;

	if (shader)
	{
		delete shader;
		shader = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

}


///////////////////////////           [ DISTANCE TO CENTER OF RADIAL GRADIENT ]           ///////////////////////////                     

float Terrain::DistanceToCenterGradient(int posX, int posZ, float radius, XMINT2 center, float steepness)
{
	// returns the distance from position X, Y to the center of a
	// defined radial gradient with a radius and a center position.
	// The returned distance is then subtracted from the height creating a cone shape.

	float distanceX = (center.x - posX) * (center.x - posX);
	float  distanceY = (center.y - posZ) * (center.y - posZ);

	//distance to center of radial gradient 
	float distanceToCenter = sqrt(distanceX * steepness + distanceY * steepness); // (h^2 = a^2 + b^2)

	if (distanceToCenter > radius) { // keep the shape of the radial gradient circular	
		distanceToCenter = radius;
	}

	return (distanceToCenter / resolution) * 100; // maintain the distance to center over varying resolutions

}



///////////////////////////           [ GENERATE HEIGHTMAP ]           ///////////////////////////                     

void Terrain::GenerateHeightMap() {

	// generates a heightmap using radial gradients and perlin noise, there are 3 different types of terrain:
	// 
	// island terrain - creates a single island using 3 radial gradients, one of them having a large radius and 
	// a central position with two smaller ones surrounding it.
	// 
	// archipelago terrain - uses 6 different radial gradients of random radii at random positions to produce an
	// archipelago type terrain, since they are random however it is possible that a single island is created.
	// 
	// volcano terrain - like the island terrain but using three radial gradients at the same position to create
	// a larger mountain from which a volcano is created (see GenerateVolcanoes func). 


	// base height of the terrain
	float height = -3.5;

	// different radii used for the gradients
	float radius1, radius2, radius3, radius4, radius5, radius6 = 0;

	// centers of the radial gradients 
	XMINT2 center1, center2, center3, center4, center5, center6;


	if (islandTerrain) {
		// setup radial gradients for a single island terrain:
		
		// central radial gradient - central island
		radius1 = (islandRadius1 * resolution) / 100; // maintaining the same radius when resolution changes
		center1.x = resolution / 2 - (rand() % (int)radius1) + (rand() % (int)radius1); // keep the island position random but close to the center 
		center1.y = resolution / 2 - (rand() % (int)radius1) + (rand() % (int)radius1);

		// surrounding radial gradients - surrounding islands 
		radius2 = ((islandRadius2)*resolution) / 100; // maintaining the same radius when resolution changes
		int maxPos2 = (resolution - radius2 * 2) - (radius2 * 2);
		center2.x = (rand() % maxPos2) + (radius2 * 2); // island center is set at random position on plane and inside the plane terrain 
		center2.y = (rand() % maxPos2) + (radius2 * 2);

		radius3 = ((islandRadius3)*resolution) / 100; // maintaining the same radius when resolution changes
		int maxPos3 = (resolution - radius3 * 2) - (radius3 * 2);
		center3.x = (rand() % maxPos3) + (radius3 * 2); // island center is set at random position on plane and inside the plane terrain
		center3.y = (rand() % maxPos3) + (radius3 * 2);
	}



	if (archipelagoTerrain) {
		// setup radial gradients for archipelago terrain:
	
		// generate random radii for 6 radial gradients (6 islands)
		islandRadius1 = (rand() % 12) + 7;
		islandRadius2 = (rand() % 12) + 7;
		islandRadius3 = (rand() % 12) + 7;
		islandRadius4 = (rand() % 12) + 7;
		islandRadius5 = (rand() % 12) + 7;
		islandRadius6 = (rand() % 12) + 7;

		// island 1
		radius1 = ((islandRadius1)*resolution) / 100; // maintaining the same radius when resolution changes
		int maxPos1 = (resolution - radius1 * 2) - (radius1 * 2); // maximum position the island can be based on its size
		center1.x = (rand() % maxPos1) + (radius1 * 2); // island center is set at random position on plane and inside the plane terrain 
		center1.y = (rand() % maxPos1) + (radius1 * 2); 
		// island 2
		radius2 = ((islandRadius2)*resolution) / 100; 
		int maxPos2 = (resolution - radius2 * 2) - (radius2 * 2);
		center2.x = (rand() % maxPos2) + (radius2 * 2); 
		center2.y = (rand() % maxPos2) + (radius2 * 2);
		// island 3
		radius3 = ((islandRadius3)*resolution) / 100; 
		int maxPos3 = (resolution - radius3 * 2) - (radius3 * 2);
		center3.x = (rand() % maxPos3) + (radius3 * 2); 
		center3.y = (rand() % maxPos3) + (radius3 * 2);
		// island 4
		radius4 = ((islandRadius4)*resolution) / 100; 
		int maxPos4 = (resolution - radius4 * 2) - (radius4 * 2);
		center4.x = (rand() % maxPos4) + (radius4 * 2);
		center4.y = (rand() % maxPos4) + (radius4 * 2);
		// island 5
		radius5 = ((islandRadius5)*resolution) / 100; 
		int maxPos5 = (resolution - radius5 * 2) - (radius5 * 2);
		center5.x = (rand() % maxPos5) + (radius5 * 2);
		center5.y = (rand() % maxPos5) + (radius5 * 2);
		// island 6
		radius6 = ((islandRadius6)*resolution) / 100; 
		int maxPos6 = (resolution - radius6 * 2) - (radius6 * 2);
		center6.x = (rand() % maxPos6) + (radius6 * 2);
		center6.y = (rand() % maxPos6) + (radius6 * 2);
	}



	if (volcanoTerrain) {
		// setup radial gradients for volcano terrain:

		islandRadius1 = 15;
		// central radial gradient 1 - central island 1
		radius1 = (islandRadius1 * resolution) / 100; // maintaining the same radius when resolution changes
		center1.x = resolution / 2 - (rand() % (int)radius1) + (rand() % (int)radius1); // keep the island position random but close to the center 
		center1.y = resolution / 2 - (rand() % (int)radius1) + (rand() % (int)radius1);

		// central radial gradient 2 - central island 2
		radius2 = (islandRadius1 * resolution) / 100;
		center2.x = center1.x; // same position of first radial gradient to give a larger mountain (cone)
		center2.y = center1.y;

		// central radial gradient 3 - central island 3
		radius3 = ((islandRadius2)*resolution) / 100; 
		int maxPos3 = (resolution - radius3 * 2) - (radius3 * 2);
		center3.x = center1.x; // same position of first radial gradient to give a larger mountain (cone)
		center3.y = center1.y;

		// surrounding radial gradient - surrounding island 
		radius4 = ((islandRadius3)*resolution) / 100; 
		int maxPos4 = (resolution - radius4 * 2) - (radius4 * 2); // maximum position the island can be based on its size
		center4.x = (rand() % maxPos4) + (radius4 * 2); // island center is set at random position on plane and inside the plane terrain
		center4.y = (rand() % maxPos4) + (radius4 * 2);
	}


	for (int z = 0; z < (resolution); z++) {
		for (int x = 0; x < (resolution); x++) {
			
			float islandGradients = 0;
			float noise = 0;

			if (islandTerrain) {
				// distances from x and y to the 3 different gradient centers are calculated 
				float distanceToCenter1 = DistanceToCenterGradient(x, z, radius1, center1, 0.8);
				float distanceToCenter2 = DistanceToCenterGradient(x, z, radius2, center2, 0.4);
				float distanceToCenter3 = DistanceToCenterGradient(x, z, radius3, center3, 0.2);

				// The amount by which the height is increased based on 3 different gradients, 
				// the closer x and z get to the center of the gradients the bigger the height
				// essentially producing 3 cones.
				islandGradients = (islandRadius1 - distanceToCenter1) + (islandRadius2 - distanceToCenter2) + (islandRadius3 - distanceToCenter3);



				// fractional brownian motion: 3 different noise octaves are added together,
				// the frequency of each is increased as the amplitude is decreased 

				float xy[2] = { z * frequencyOct1, x * frequencyOct1 };
				float xz[2] = { z * frequencyOct2, x * frequencyOct2 };
				float xx[2] = { z * frequencyOct3, x * frequencyOct3 };

				noise = perlin.noise(xy[0], xy[1]) * amplitudeOct1 
					  + perlin.noise(xz[0], xz[1]) * amplitudeOct2
					  + perlin.noise(xx[0], xx[1]) * amplitudeOct3;

			}

			else if (archipelagoTerrain) {

				// distances from x and y to the 6 different gradient centers are calculated 
				float distanceToCenter1 = DistanceToCenterGradient(x, z, radius1, center1, 1);
				float distanceToCenter2 = DistanceToCenterGradient(x, z, radius2, center2, 1);
				float distanceToCenter3 = DistanceToCenterGradient(x, z, radius3, center3, 1);
				float distanceToCenter4 = DistanceToCenterGradient(x, z, radius4, center4, 1);
				float distanceToCenter5 = DistanceToCenterGradient(x, z, radius5, center5, 1);
				float distanceToCenter6 = DistanceToCenterGradient(x, z, radius6, center6, 1);


				// The amount by which the height is increased based on 6 different gradients, 
				// the closer x and z get to the center of the gradients the bigger the height
				// essentially producing 6 cones.
				islandGradients = (islandRadius1 - distanceToCenter1) + (islandRadius2 - distanceToCenter2) + (islandRadius3 - distanceToCenter3) + (islandRadius4 - distanceToCenter4) + (islandRadius5 - distanceToCenter5) + (islandRadius6 - distanceToCenter6);



				// fractional brownian motion: 3 different noise octaves are added together,
				// the frequency of each is increased as the amplitude is decreased 
				float xy[2] = { z * frequencyOct1, x * frequencyOct1 };
				float xz[2] = { z * frequencyOct2, x * frequencyOct2 };
				float xx[2] = { z * frequencyOct3, x * frequencyOct3 };

				noise = perlin.noise(xy[0], xy[1]) * amplitudeOct1
					  + perlin.noise(xz[0], xz[1]) * amplitudeOct2
					  + perlin.noise(xx[0], xx[1]) * amplitudeOct3;

			}



			if (volcanoTerrain) {
				// distances from x and y to the 4 different gradient centers are calculated, the first three have the same center
				float distanceToCenter1 = DistanceToCenterGradient(x, z, radius1, center1, 0.8);
				float distanceToCenter2 = DistanceToCenterGradient(x, z, radius2, center2, 0.8);
				float distanceToCenter3 = DistanceToCenterGradient(x, z, radius3, center3, 0.2);
				float distanceToCenter4 = DistanceToCenterGradient(x, z, radius4, center4, 0.2);

				// The amount by which the height is increased based on 4 different gradients, 
				// the closer x and z get to the center of the gradients the bigger the height
				// essentially producing 4 cones. The center of the 3 first cones is set to the
				// same coordinates so the height will have a larger value.
				islandGradients = (islandRadius1 - distanceToCenter1) + (islandRadius1 - distanceToCenter2) + (islandRadius2 - distanceToCenter3) + (islandRadius3 - distanceToCenter4);



				// fractional brownian motion: 3 different noise octaves are added together,
				// the frequency of each is increased as the amplitude is decreased 
				float xy[2] = { z * frequencyOct1, x * frequencyOct1 };
				float xz[2] = { z * frequencyOct2, x * frequencyOct2 };
				float xx[2] = { z * frequencyOct3, x * frequencyOct3 };

				noise = perlin.noise(xy[0], xy[1]) * amplitudeOct1
				      + perlin.noise(xz[0], xz[1]) * amplitudeOct2
					  + perlin.noise(xx[0], xx[1]) * amplitudeOct3;

			}

			// populate the heightmap
			heightMap[(z * resolution) + x] = height + noise + islandGradients;
		}
	}

}





///////////////////////////           [ RESIZE TERRAIN ]           ///////////////////////////   

void Terrain::Resize(int newResolution) {
	resolution = newResolution;
	if (vertexBuffer != NULL) {
		vertexBuffer->Release();
	}
	vertexBuffer = NULL;
}

///////////////////////////           [ REGENERATE TERRAIN ]           ///////////////////////////                     

void Terrain::Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	// Set up the heightmap and create or update the appropriate buffers

	islandRadius1 = 20;
	islandRadius2 = 16;
	islandRadius3 = 13;

	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, height, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Calculate and store the height values

	// Calculate the number of vertices in the terrain mesh.
	// We share vertices in this mesh, so the vertex count is simply the terrain 'resolution'
	// and the index count is the number of resulting triangles * 3 OR the number of quads * 6
	vertexCount = resolution * resolution;
	indexCount = ((resolution - 1) * (resolution - 1)) * 6;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	index = 0;
	// UV coords.
	u = 0;
	v = 0;
	increment = m_UVscale / resolution;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;

	//Set up vertices
	for (j = 0; j < (resolution); j++) {
		for (i = 0; i < (resolution); i++) {

			positionX = (float)i * scale;
			positionZ = (float)(j)*scale;

			height = heightMap[index];
			vertices[index].position = XMFLOAT3(positionX, height, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
	}

	//Set up index list
	index = 0;
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {

			//Build index array
			indices[index] = (j * resolution) + i;
			indices[index + 1] = ((j + 1) * resolution) + (i + 1);
			indices[index + 2] = ((j + 1) * resolution) + i;

			indices[index + 3] = (j * resolution) + i;
			indices[index + 4] = (j * resolution) + (i + 1);
			indices[index + 5] = ((j + 1) * resolution) + (i + 1);
			index += 6;
		}
	}

	//Set up normals
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {
			//Calculate the plane normals
			XMFLOAT3 a, b, c;	//Three corner vertices
			a = vertices[j * resolution + i].position;
			b = vertices[j * resolution + i + 1].position;
			c = vertices[(j + 1) * resolution + i].position;

			//Two edges
			XMFLOAT3 ab(c.x - a.x, c.y - a.y, c.z - a.z);
			XMFLOAT3 ac(b.x - a.x, b.y - a.y, b.z - a.z);

			//Calculate the cross product
			XMFLOAT3 cross;
			cross.x = ab.y * ac.z - ab.z * ac.y;
			cross.y = ab.z * ac.x - ab.x * ac.z;
			cross.z = ab.x * ac.y - ab.y * ac.x;
			float mag = (cross.x * cross.x) + (cross.y * cross.y) + (cross.z * cross.z);
			mag = sqrtf(mag);
			cross.x /= mag;
			cross.y /= mag;
			cross.z /= mag;
			vertices[j * resolution + i].normal = cross;
		}
	}

	//Smooth the normals by averaging the normals from the surrounding planes
	XMFLOAT3 smoothedNormal(0, 1, 0);
	for (j = 0; j < resolution; j++) {
		for (i = 0; i < resolution; i++) {
			smoothedNormal.x = 0;
			smoothedNormal.y = 0;
			smoothedNormal.z = 0;
			float count = 0;
			//Left planes
			if ((i - 1) >= 0) {
				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[j * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[j * resolution + (i - 1)].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + (i - 1)].normal.z;
					count++;
				}
			}
			//right planes
			if ((i) < (resolution - 1)) {

				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + i].normal.x;
					smoothedNormal.y += vertices[j * resolution + i].normal.y;
					smoothedNormal.z += vertices[j * resolution + i].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + i].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + i].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + i].normal.z;
					count++;
				}
			}
			smoothedNormal.x /= count;
			smoothedNormal.y /= count;
			smoothedNormal.z /= count;

			float mag = sqrt((smoothedNormal.x * smoothedNormal.x) + (smoothedNormal.y * smoothedNormal.y) + (smoothedNormal.z * smoothedNormal.z));
			smoothedNormal.x /= mag;
			smoothedNormal.y /= mag;
			smoothedNormal.z /= mag;

			vertices[j * resolution + i].normal = smoothedNormal;
		}
	}



	
	if (volcanoTerrain) {
		// generate volcano craters on mountains high enough 
		GenerateVolcanoes(vertices);
	}


	// Apply hydraulic erosion to the terrain
	ApplyErosion(vertices);



	// Re-calculate normals  
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {
			//Calculate the plane normals
			XMFLOAT3 a1, b1, c1;	//Three corner vertices
			a1 = vertices[j * resolution + i].position;
			b1 = vertices[j * resolution + i + 1].position;
			c1 = vertices[(j + 1) * resolution + i].position;

			//Two edges
			XMFLOAT3 ab(c1.x - a1.x, c1.y - a1.y, c1.z - a1.z);
			XMFLOAT3 ac(b1.x - a1.x, b1.y - a1.y, b1.z - a1.z);

			//Calculate the cross product
			XMFLOAT3 cross1;
			cross1.x = ab.y * ac.z - ab.z * ac.y;
			cross1.y = ab.z * ac.x - ab.x * ac.z;
			cross1.z = ab.x * ac.y - ab.y * ac.x;
			float mag1 = (cross1.x * cross1.x) + (cross1.y * cross1.y) + (cross1.z * cross1.z);
			mag1 = sqrtf(mag1);
			cross1.x /= mag1;
			cross1.y /= mag1;
			cross1.z /= mag1;
			vertices[j * resolution + i].normal = cross1;
		}
	}

	// Smooth the normals by averaging the normals from the surrounding planes
	XMFLOAT3 smoothedNormal1(0, 1, 0);
	for (j = 0; j < resolution; j++) {
		for (i = 0; i < resolution; i++) {
			smoothedNormal1.x = 0;
			smoothedNormal1.y = 0;
			smoothedNormal1.z = 0;
			float count1 = 0;
			//Left planes
			if ((i - 1) >= 0) {
				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal1.x += vertices[j * resolution + (i - 1)].normal.x;
					smoothedNormal1.y += vertices[j * resolution + (i - 1)].normal.y;
					smoothedNormal1.z += vertices[j * resolution + (i - 1)].normal.z;
					count1++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal1.x += vertices[(j - 1) * resolution + (i - 1)].normal.x;
					smoothedNormal1.y += vertices[(j - 1) * resolution + (i - 1)].normal.y;
					smoothedNormal1.z += vertices[(j - 1) * resolution + (i - 1)].normal.z;
					count1++;
				}
			}
			//right planes
			if ((i) < (resolution - 1)) {

				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal1.x += vertices[j * resolution + i].normal.x;
					smoothedNormal1.y += vertices[j * resolution + i].normal.y;
					smoothedNormal1.z += vertices[j * resolution + i].normal.z;
					count1++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal1.x += vertices[(j - 1) * resolution + i].normal.x;
					smoothedNormal1.y += vertices[(j - 1) * resolution + i].normal.y;
					smoothedNormal1.z += vertices[(j - 1) * resolution + i].normal.z;
					count1++;
				}
			}
			smoothedNormal1.x /= count1;
			smoothedNormal1.y /= count1;
			smoothedNormal1.z /= count1;

			float mag1 = sqrt((smoothedNormal1.x * smoothedNormal1.x) + (smoothedNormal1.y * smoothedNormal1.y) + (smoothedNormal1.z * smoothedNormal1.z));
			smoothedNormal1.x /= mag1;
			smoothedNormal1.y /= mag1;
			smoothedNormal1.z /= mag1;

			vertices[j * resolution + i].normal = smoothedNormal1;
		}
	}



	//If we've not yet created our dyanmic Vertex and Index buffers, do that now
	if (vertexBuffer == NULL) {
		CreateBuffers(device, vertices, indices);
	}
	else {
		//If we've already made our buffers, update the information
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//  Disable GPU access to the vertex buffer data.
		deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, vertices, sizeof(VertexType) * vertexCount);
		//  Reenable GPU access to the vertex buffer data.
		deviceContext->Unmap(vertexBuffer, 0);
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}


///////////////////////////           [ HYDRAULIC EROSION ]           ///////////////////////////                     

void Terrain::HydraulicErosion(float x, float y, VertexType* vertices)
{
	// simulates a dropplet falling at position x, y on the terrain. Dropplets
	// start small and gain material as the roll down the terrain, this
	// material is deposited on the terrain as it rolls down, the dropplets
	// stop rolling when the terrain is flat. 


    float sediment = 0; // amount of solid terrain material moved
    float xprev = x; // previous x position
	float yprev = y; // previous y position
    float vx = 0;    // horizontal velocity
    float vy = 0;    // vertical velocity 

	for (int i = 0; i < maxIterations; ++i) { // number of iterations per dropplet

		int j = floor(y);
		int ix = floor(x);

		float surfaceNormalX = 0;
		float surfaceNormalY = 1;
		float surfaceNormalZ = 0;

		// get the surface normal of the terrain at the current location
		if (j < resolution && ix < resolution) {
			if (j >= 0 && ix >= 0) { // if x and y are in the terrain grid
				 surfaceNormalX = vertices[j * resolution + ix].normal.x;
				 surfaceNormalY = vertices[j * resolution + ix].normal.y;
				 surfaceNormalZ = vertices[j * resolution + ix].normal.z;
			}
		}


		// if the terrain is flat stop the dropplet simulation, stops rolling down. 
		if (surfaceNormalY >= 0.99f) { 
			break;
		}

		// calculate the amount of deposited terrain based on the slope of the terrain and the deposition rate
		float depositedTerrain = sediment * depositionRate * surfaceNormalY; 

		// calculate the amount of terrain that is transported (eroded) based on the erosion rate, the slope
		// and the minimum of i*iterationScale and 1 which increases the amount of terrain eroded as the dropplet
		// rolls further down until it reaches its largest 
		float erosion = erosionRate * (1 - surfaceNormalY) * min(1,  i * iterationScale);


		// amount of terrain deposited and eroded at current location
		float delta = (depositedTerrain - erosion) * deltaMultiplier;


		if (yprev >= 0 && xprev >= 0) { 
			int xi = floor(xprev);
			int yi = floor(yprev);
			// if the current location xp, xy is in the terrain
			if (xi < resolution-1 && yi < resolution-1) {
				float fx = xprev - xi;
				float fy = yprev - yi;
				// deform the current location on the terrain and the surrounding vertices 
				vertices[xi + yi * resolution].position.y += fx * fy * delta;
				vertices[xi + 1 + yi * resolution].position.y += (1 - fx) * fy * delta; 
				vertices[xi + (yi + 1) * resolution].position.y += fx * (1 - fy) * delta;
				vertices[xi + 1 + (yi + 1) * resolution].position.y += (1 - fx) * (1 - fy) * delta;
			}
		}	

		// advance the position of the dropplet based on the speed and slope of terrain 
		vx = (friction * vx) + surfaceNormalX * speed; 
		vy = (friction * vy) + surfaceNormalZ * speed; 
	
		// set previous positions of the dropplet
		xprev = x;
		yprev = y;
		x += vx;
		y += vy;

		// increase the amount of terrain moved by the droplet
		sediment += (erosion - depositedTerrain) * 0.05;
	}

}


///////////////////////////           [ BLUR TERRAIN ]           ///////////////////////////                     

void Terrain::BlurTerrain(VertexType* vertices)
{
	// apply a gaussian blur to the vertices of the terrain, this is done
	// by setting the height value of the vertex to that of the weighted 
	// height values of the surrounding vertices. Following a gaussian curve
	// meaning that the further most vertices affect less the center vertex.
	// 
	// height at x, y is sum of heights:
	// _________________________________________________________________
	//|                     |                     |                     |
	//|  (x-1, y+1)*0.0625  |   (x, y+1)*0.125    |  (x+1, y+1)*0.0625  |
	//|_____________________|_____________________|_____________________|
	//|                     |                     |                     |
    //|   (x-1, y)*0.125    |     (x, y)*0.25     |   (x+1, y)*0.125    |
	//|_____________________|_____________________|_____________________|
	//|                     |                     |                     |
    //|  (x-1, y-1)*0.0625  |   (x, y-1)*0.125    |  (x+1, y-1)*0.0625  |
	//|_____________________|_____________________|_____________________|


	for (int j = 1; j < (resolution - 1); j++) {
		for (int i = 1; i < (resolution - 1); i++) {
			vertices[j * resolution + i].position.y = 
					(vertices[i - 1 + j * resolution].position.y +
					 vertices[i + (j - 1) * resolution].position.y +
					 vertices[i + 1 + j * resolution].position.y +
					 vertices[i + (j + 1) * resolution].position.y) * 0.125 					
					+ 
					(vertices[i - 1 + (j - 1) * resolution].position.y +
					vertices[i + 1 + (j - 1) * resolution].position.y +
					vertices[i + 1 + (j + 1) * resolution].position.y +
					vertices[i - 1 + (j + 1) * resolution].position.y) * 0.0625 					
					+
					vertices[i + j * resolution].position.y * 0.25;
			}	
	}

}



///////////////////////////           [ APPLY EROSION ]           ///////////////////////////                     
void Terrain::ApplyErosion(VertexType* vertices)
{
	// simulate hydraulic erosion
	for (int i = 0; i < dropplets; i++) {
		HydraulicErosion(rand() % (resolution), rand() % (resolution), vertices);
	}

	// blur the terrain 
	for (int i = 0; i < blurTimes; i++) {
		BlurTerrain(vertices);
	}
}


///////////////////////////           [ GENERATE VOLCANOES ]           /////////////////////////// 
void Terrain::GenerateVolcanoes(VertexType* vertices)
{
	// if the height of the vertices is above a defined threshold the position is inverted
	// giving a crater like shape

	for (int j = 1; j < (resolution - 1); j++) {
		for (int i = 1; i < (resolution - 1); i++) {
			if (vertices[j * resolution + i].position.y >= 40) { // height is above threshold
				vertices[j * resolution + i].position.y = 35 + (40 - (vertices[j * resolution + i].position.y));
			}
		}
	}
}

///////////////////////////           [ RENDER ]           /////////////////////////// 
void Terrain::render(XMMATRIX& camview, Light* light, XMFLOAT3 camPos, float dirBrightness, bool normals){
	XMMATRIX worldMatrix, projectionMatrix;
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50, 0, -50); // position at which the terrain is rendered 
	projectionMatrix = renderer->getProjectionMatrix();
	this->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projectionMatrix, textures, light, dirBrightness, normals, camPos, volcanoTerrain);
	shader->render(renderer->getDeviceContext(), this->getIndexCount());

}


                    
///////////////////////////           [ RENDER IN BLACK ]           /////////////////////////// 
void Terrain::renderInBlack(XMMATRIX& camview, XMFLOAT3 camPos , bool normals)
{
	XMMATRIX worldMatrix, projectionMatrix;
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50, 0, -50);
	projectionMatrix = renderer->getProjectionMatrix();

	this->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, renderer->getProjectionMatrix(), textureMgr->getTexture(L"black"), normals);
	textureShader->render(renderer->getDeviceContext(), this->getIndexCount());
}



///////////////////////////           [ USER INTERFACE ]           /////////////////////////// 
void Terrain::GUI()
{
	if (ImGui::CollapsingHeader("Terrain Settings")) {
		if (ImGui::Checkbox("Island", &islandTerrain)) {
			archipelagoTerrain = false;
			volcanoTerrain = false;
			Resize(resolution);
			heightMap = new float[resolution * resolution];
			GenerateHeightMap();
			Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}

		if (ImGui::Checkbox("Archipelago", &archipelagoTerrain)) {
			islandTerrain = false;
			volcanoTerrain = false;
			Resize(resolution);
			heightMap = new float[resolution * resolution];
			GenerateHeightMap();
			Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}

		if (ImGui::Checkbox("Volcano", &volcanoTerrain)) {
			islandTerrain = false;
			archipelagoTerrain = false;
			Resize(resolution);
			heightMap = new float[resolution * resolution];
			GenerateHeightMap();
			Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		
		if (ImGui::Button(" Generate Terrain")) {
			Resize(resolution);
			heightMap = new float[resolution * resolution];
			GenerateHeightMap();
			Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}


		if (ImGui::CollapsingHeader("Perlin Noise Settings")) {

			bool regen = false;
			ImGui::Text("Noise Octave 1");
			if (ImGui::SliderFloat(" Octave 1 frequency", &frequencyOct1, 0.01, 1.f)) {
				regen = true;
			}

			if (ImGui::SliderFloat(" Octave 1 amplitude", &amplitudeOct1, 2, 20.f)) {
				regen = true;
			}
			ImGui::Text("Noise Octave 2");
			if (ImGui::SliderFloat(" Octave 2 frequency", &frequencyOct2, 0.01, 1.f)) {
				regen = true;

			}
			if (ImGui::SliderFloat(" Octave 2 amplitude", &amplitudeOct2, 1, 20.f)) {
				regen = true;
			}

			ImGui::Text("Noise Octave 3");
			if (ImGui::SliderFloat(" Octave 3 frequency", &frequencyOct3, 0.01, 1.f)) {
				regen = true;

			}
			if (ImGui::SliderFloat(" Octave 3 amplitude", &amplitudeOct3, 0.5, 20.f)) {
				regen = true;
			}

		
			if (ImGui::Button(" Noise Default Values", ImVec2(170, 20))) {
				frequencyOct1 = 0.02f;
				frequencyOct2 = 0.03f;
				frequencyOct3 = 0.10f;
				amplitudeOct1 = 2.0f;
				amplitudeOct2 = 1.0f;
				amplitudeOct3 = 0.5f;
				regen = true;
			}

			if (regen) {
				Resize(resolution);
				heightMap = new float[resolution * resolution];
				GenerateHeightMap();
				Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			}

		}



		if (ImGui::CollapsingHeader("Hydraulic Erosion Settings")) {
			bool regen = false;
			if (ImGui::SliderInt(" Amount of dropplets", &dropplets, 3500, 100000)) {
				regen = true;
			}

			if (ImGui::SliderFloat(" Dropplet speed", &speed, 0.001, 4.f)) {
				regen = true;
			}

			if (ImGui::SliderFloat(" Friction", &friction, 0.001, 2.f)) {
				regen = true;
			}

			if (ImGui::SliderFloat(" Amount of deposited terrain", &depositionRate, 0.001, 4.f)) {
				regen = true;
			}

			if (ImGui::SliderFloat(" Erosion Amount", &erosionRate, 0.001, 50.f)) {
				regen = true;
			}

			if (ImGui::Button(" Erosion Default Values", ImVec2(170, 20))) {
				depositionRate = 0.149;
				erosionRate = 9.8;
				speed = 1.1948;
				friction = 0.399;
				dropplets = 3500;
				regen = true;
			}

			if (regen) {
				Resize(resolution);
				Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				regen = false;
			}
		}

	}
}








//Create the vertex and index buffers that will be passed along to the graphics card for rendering
void Terrain::CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices) {

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;


	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}