//This class is only meant as a initial thing to use as billboarding/intersecting quads

#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"

class GrassBush : public SceneNode
{
public: 
	GrassBush(Mesh* mesh);
	~GrassBush(void) {};
	void Update(float dt) override;

protected:
	Vector3 pos;
	Mesh* quad;
};

