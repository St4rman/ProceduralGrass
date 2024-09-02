#include "HeightMap.h"

HeightMap::HeightMap(const std::string& name, Vector3 vScale) {

	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(),
		&iWidth, &iHeight, &iChans, 1);

	if (!data) {
		std::cout << "heightmap cant load! \n";
		init = false;
		return;
	}

	numVertices		= iWidth * iHeight;
	numIndices		= (iWidth - 1) * (iHeight - 1) * 6;
	vertices		= new Vector3[numVertices];
	textureCoords	= new Vector2[numIndices];
	indices			= new GLuint[numIndices];

	Vector3 vertexScale  = vScale;
	Vector2 textureScale = Vector2(1 / vScale.x, 1 / vScale.z);

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {

			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;

			vertOffset.push_back(data[offset] * vertexScale.y);

		}
	}
	SOIL_free_image_data(data);

	int i = 0;

	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {

			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;

		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);

	init = true;
}
