#pragma once
#include "Renderer.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/ComputeShader.h"
#include <algorithm>
#include "imgui.h"
#include "imgui_impl_opengl3.h"

class Camera;
class SceneNode;
class Shader;
class HeightMap;


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	Renderer(Window& parent, int _density);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;
	
	void updateWindSpeed (float speed)			{ windSpeed	 = speed; }
	void updateWindDir	 (Vector2 newDir)		{ windDir	 = newDir;  }
	Vector3 getCamPos()							{ return camera->GetPosition(); }
	void setDensity(int x) { density = x; }

protected:

	bool initTextures();
	bool initShaders();
	bool initMeshes(int _d);
	bool initComputeShaders();

	void DrawHeightMap();
	void DrawSkybox();
	void DrawGrass();

	Vector2 SCALE = { 0,0 };
	int		TOTALDISPATCH = 0;
	int		density = 0;
	Vector3 heightmapSize;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* gpuShader;

	ComputeShader* compShader;
	Frustum frameFrustum;

	vector<SceneNode*> nodeList;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* triangle;
	Mesh* grassMesh;

	Light* light;
	Camera* camera;
	GameTimer* timer;

	Vector3 localOrigin;

	GLuint cubeMap		= 0;
	GLuint earthTex		= 0;
	GLuint earthBump	= 0;
	GLuint grassTex		= 0;
	GLuint compVnoise	= 0;
	GLuint windTex		= 0;

	GLuint ssboID	=0;
	GLuint heightBuffer = 0;

	float windSpeed;
	float windFwdSway;
	float windRightSway;
	Vector2 windDir;
	Vector3 grassDimensions;
	Vector3 cameraStartPos;
};

