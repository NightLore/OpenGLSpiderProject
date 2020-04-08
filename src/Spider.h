#pragma once

#include <iostream>

#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

class Spider
{
public:
	Spider();
	~Spider();

	std::shared_ptr<Shape> sphere;

	void initialize(std::shared_ptr<Shape> sphere);
	void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, float time);
	void drawEyes(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, float headRadius, float offset);
	void drawLegs(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, float time);
	//void drawLeg(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M,
	//	glm::vec3 rotations, glm::vec3 translate, glm::vec3 scale);
};