#include "Renderer.h"

Renderer::Renderer(Window& parent, int _density) : OGLRenderer(parent) {


	switch (_density)
	{
	case 1:
		SCALE = { 40, 40 };
		break;
	case 2:
		SCALE = {80, 80};
		break;
	default:
		SCALE = { 20 , 20 };
		break;
	}

	TOTALDISPATCH = SCALE.x * SCALE.y * 20 * 20;
	cameraStartPos = { 2402.71f, 340.03f, -3900.0f };

	if (!initShaders())  return;
	if (!initTextures()) return;
	if (!initMeshes(_density))   return;
	if (!initComputeShaders()) return;
	
	heightmapSize = heightMap->GetHeightmapSize();

	camera = new Camera(-6.4f, 161.0f, cameraStartPos);
	light = new Light(heightmapSize * Vector3(0.5f, 3.0, 0.5f), Vector4(1, 1, 1, 1), 10000);

	localOrigin = heightmapSize * Vector3(0.5f, 5.0f, 0.5f);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	timer = parent.GetTimer();
	
	windSpeed = 0.02;
	windDir   = { 0 , 1 };
	windFwdSway = 90;
	windRightSway = 70;

	grassDimensions = { 4, 15, 4 };
	init = true;
	
}

Renderer::~Renderer(void) {

	delete heightMap;
	delete quad;
	delete triangle;
	delete grassMesh;
	delete light;
	delete camera;
	delete timer;

	//ALL SHADERS!!!
	delete lightShader;
	delete skyboxShader;
	delete sceneShader;
	delete gpuShader;
	delete compShader;

	//DELETE ALL TEXTURES!!!
	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &earthTex);
	glDeleteTextures(1, &earthBump);
	glDeleteTextures(1, &grassTex);
	glDeleteTextures(1, &windTex);
	glDeleteTextures(1, &compVnoise);
}


bool Renderer::initTextures() {

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"base_grass.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"base_grassN.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grassbush.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	windTex  = SOIL_load_OGL_texture(TEXTUREDIR"wind-bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"left.png", TEXTUREDIR"right.png",
		TEXTUREDIR"up.png", TEXTUREDIR"down.png",
		TEXTUREDIR"front.png", TEXTUREDIR"back.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!earthTex || !earthBump || !cubeMap || !grassTex || !windTex) {
		return false;
	}

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	return true;

}

bool Renderer::initShaders() {

	skyboxShader  = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader   = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	sceneShader   = new Shader("sceneVertex.glsl", "sceneFragment.glsl");
	gpuShader     = new Shader("gpuSceneVertex.glsl", "gpuSceneFragment.glsl");
	
	if (!skyboxShader->LoadSuccess() ||!lightShader->LoadSuccess()   || 
		!sceneShader->LoadSuccess()  ||!gpuShader->LoadSuccess() ) {
		return false;
	}
	return true;
}

bool Renderer::initComputeShaders() {

	compShader = new ComputeShader("positionCompute.glsl");

	if (!compShader->LoadSuccess()) return false;

	glCreateBuffers(1, &ssboID);
	glNamedBufferStorage(ssboID, 3 * TOTALDISPATCH * sizeof(GLfloat) * 4 * sizeof(GLfloat), 0, GL_DYNAMIC_STORAGE_BIT);


	if (!heightMap->loadSuccess()) return false;
	

	std::vector<float> temp = heightMap->GetVerticalOffset();
	std::reverse(temp.begin(), temp.end());
	glCreateBuffers(1, &heightBuffer);
	glNamedBufferStorage(heightBuffer, temp.size() * sizeof(GLfloat),reinterpret_cast<GLfloat*>(temp.data()), GL_DYNAMIC_STORAGE_BIT);



	glGenTextures(1, &compVnoise);
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compVnoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 200, 200, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, compVnoise, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);


	return true;
}

bool Renderer::initMeshes(int _d){

	quad		= Mesh::GenerateQuad();
	triangle	= Mesh::GenerateTriangle();
	triangle->GenerateNormals();
	triangle->GenerateTangents();
	switch (_d)
	{
	case 1:
		heightMap = new HeightMap(TEXTUREDIR"baseMed.png", { 10.0f, 5.0f, 10.0f });
		break;
	case 2:
		heightMap = new HeightMap(TEXTUREDIR"baseBig.png", { 10.0f, 5.0f, 10.0f });
		break;
	default:
		heightMap = new HeightMap(TEXTUREDIR"base.jpg", { 10.0f, 5.0f, 10.0f });
		break;
	}
	

	grassMesh	= Mesh::LoadFromMeshFile("GrassVert.msh");

	return heightMap->loadSuccess();

}


void Renderer::UpdateScene(float dt) {

	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	
}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightMap();
	DrawGrass();
	
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {

	BindShader(lightShader);

	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	Vector3 hSize = heightMap->GetHeightmapSize();
	
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation({ 0,0,0 })* Matrix4::Rotation(90, { 0,1,0 }) ;
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->DrawInstanced(1);

}

void Renderer::DrawGrass() {


	Vector3 hSize = heightMap->GetHeightmapSize();

	Vector3 grassOffset = - hSize / grassDimensions;
	grassOffset.y = 1.0f;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboID);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, heightBuffer);

	compShader->Bind();
	compShader->Dispatch((unsigned int)SCALE.x, (unsigned int)SCALE.y, 1);


	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUniform3fv(glGetUniformLocation(compShader->GetProgram(), "mapSize"), 1, (float*)&hSize);
	glUniform1f(glGetUniformLocation(compShader->GetProgram(), "t"), (float)timer->GetTotalTimeSeconds());
	glUniform2fv(glGetUniformLocation(compShader->GetProgram(), "density"), 1, (float*)&SCALE);
	glUniform1f(glGetUniformLocation(compShader->GetProgram(), "windSpeed"), (float)windSpeed);
	glUniform2fv(glGetUniformLocation(compShader->GetProgram(), "windDir"), 1, (float*)&windDir);
	glUniform3fv(glGetUniformLocation(compShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform3fv(glGetUniformLocation(compShader->GetProgram(), "grassDims"), 1, (float*)&grassDimensions);
	glUniform3fv(glGetUniformLocation(compShader->GetProgram(), "offset"), 1, (float*)&grassOffset);


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	BindShader(gpuShader);
	SetShaderLight(*light);

	modelMatrix = Matrix4::Translation({ 0 , 0, 0}) * Matrix4::Scale(grassDimensions);
	textureMatrix.ToIdentity();

	
	
	glUniform1i(glGetUniformLocation(gpuShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(gpuShader->GetProgram(), "windTex"),	 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compVnoise);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, windTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	

	glUniform1i(glGetUniformLocation(gpuShader->GetProgram(), "useTexture"), 0);


	glUniform3fv(glGetUniformLocation(gpuShader->GetProgram(), "spacePerBlade"), 1, (float*)&grassOffset);
	glUniform1f(glGetUniformLocation(gpuShader->GetProgram(), "time"), (float)timer->GetTotalTimeSeconds());
	glUniform1f(glGetUniformLocation(gpuShader->GetProgram(), "windFwdSway"), (float)windFwdSway);
	glUniform1f(glGetUniformLocation(gpuShader->GetProgram(), "windRightSway"), (float)windRightSway);
	glUniform2fv(glGetUniformLocation(gpuShader->GetProgram(), "windDir"), 1, (float*)&windDir);
	glUniform3fv(glGetUniformLocation(gpuShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform3fv(glGetUniformLocation(gpuShader->GetProgram(), "grassDims"), 1, (float*)&grassDimensions);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UpdateShaderMatrices();
	triangle->DrawInstanced(TOTALDISPATCH);	
}