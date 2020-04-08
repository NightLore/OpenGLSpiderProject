#pragma once

#include <iostream>
#include <glad/glad.h>
#include <time.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "stb_image.h"


using namespace std;
using namespace glm;

class Entity
{
public:
	Entity();
	~Entity();
	vec3 location;
	int type;
	float direction;
	float size;
	float timeFrame;
	int material;
	vec3 maxBounds;
	vec3 minBounds;
	void reset(int type);
	void spawn(int new_type, vec3 max, vec3 min, float max_size, float min_size);
	void move(vec3 dir, Entity *entities, int num_entities);
	bool hasCollided(Entity b);
	void draw(shared_ptr<MatrixStack> P, mat4 V, shared_ptr<MatrixStack> M);
private:
	bool hasCollided(Entity *entities, int num_entities);
	void extractOuterPoints(vec3 points[]);
	bool isInsideBounds(vec3 pt, vec3 max, vec3 min);

};

