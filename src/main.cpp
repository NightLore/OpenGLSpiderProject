/*
Base code
Currently will make 2 FBOs and textures (only uses one in base code)
and writes out frame as a .png (Texture_output.png)

Winter 2017 - ZJW (Piddington texture write)
2017 integration with pitch and yaw camera lab (set up for texture mapping lab)
*/

#include <iostream>
#include <glad/glad.h>
#include <time.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"
#include "Entity.h"
#include "Constants.h"

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

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> texProg;
	std::shared_ptr<Program> tex0Prog;
	std::shared_ptr<Program> tex1Prog;
	std::shared_ptr<Program> cubeProg;

	// Shape to be used (from obj file)
	//shared_ptr<Shape> shapeNef;
	shared_ptr<Shape> sphere;
	shared_ptr<Shape> world;
	shared_ptr<Shape> cube;
	//shared_ptr<Shape> dog;
	//shared_ptr<Shape> flower;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//geometry for texture render
	GLuint quad_VertexArrayID;
	GLuint quad_vertexbuffer;

	//reference to texture FBO
	GLuint frameBuf[2];
	GLuint texBuf[2];
	GLuint depthBuf;
	
	shared_ptr<Texture> textures[NUMTEX];
	unsigned int cubeTextures[NUM_CUBE_TEX];

	//For each shape, now that they are not resized, they need to be
	//transformed appropriately to the origin and scaled
	//transforms for Nef
	vec3 gTrans = vec3(0);
	float gScale = 1.0;

	//transforms for the world
	vec3 gDTrans = vec3(0);
	float gDScale = 1.0;

	bool FirstTime = true;
	bool Moving = false;
	int gMat = 0;

	float dy = 0;

	bool mouseDown = false;
	int drawMode = 0;
	int moveKeys[NUM_MOVEKEYS] = { GLFW_KEY_W,
								   GLFW_KEY_A,
								   GLFW_KEY_S,
								   GLFW_KEY_D,
								   GLFW_KEY_Q,
								   GLFW_KEY_E };
	bool moveKeyAction[NUM_MOVEKEYS] = { GLFW_RELEASE,
								  	     GLFW_RELEASE,
										 GLFW_RELEASE,
										 GLFW_RELEASE,
										 GLFW_RELEASE,
										 GLFW_RELEASE };

	Entity player;
	Entity entities[NUM_ENTITIES];
	vector<shared_ptr<Entity>> cobwebs;

	int view = 0;
	vec3 lookAt = -ZAXIS;
	vec3 upVector = YAXIS;

	void getDefaultBounds(int type, vec3 *max, vec3 *min)
	{
		switch(type)
		{
			case TYPE_SPIDER:
				*max = vec3(0.1, 0.1, 0.1);
				*min = -*max;
				break;
			case TYPE_WATERMELON:
				*max = sphere->max * defaultWatermelon;
				*min = sphere->min * defaultWatermelon;
				break;
			case TYPE_CRATE:
				*max = cube->max;
				*min = cube->min;
				break;
			case TYPE_COBWEB:
				*max = vec3(0.1, 0.1, 0.1);
				*min = -*max;
				break;
			default:
				*max = vec3(0.5, 0.5, 0.5);
				*min = -*max;
				break;
		}
	}

	void resetPlayer()
	{
		player.reset(TYPE_SPIDER);
		getDefaultBounds(player.type, &player.maxBounds, &player.minBounds);
	}

	void addCobweb()
	{
		shared_ptr<Entity> cobweb = make_shared<Entity>();
		cobweb->type = TYPE_COBWEB;
		cobweb->direction = rand() % 360 / 180.0 * M_PI;
		cobweb->location = player.location;
		cobweb->material = 0;
		cobweb->size = rand() % 6 + 3;
		cobweb->timeFrame = 0;
		cobwebs.push_back(cobweb);
	}

	void setRandomEntities(Entity *entities, vec3 max, vec3 min, float max_size, float min_size)
	{
		srand(time(NULL));
		for (int i = 0; i < NUM_ENTITIES; ++i) {
			spawnEntity(&entities[i], rand() % NUM_TYPES, max, min, max_size, min_size);
		}
	}

	void spawnEntity(Entity *e, int type, vec3 max, vec3 min, float max_size, float min_size)
	{
		getDefaultBounds(type, &e->maxBounds, &e->minBounds);
		e->spawn(type, max, min, max_size, min_size);
		e->material = rand() % NUM_MAT[type];
		while (e->hasCollided(player)) {
			e->location.x += e->size * 1.5;
		}
	}

	void calcIJK(vec3* i, vec3* j, vec3 *k, float scale, vec3 camDistance, vec3 upVector)
	{
		vec3 lookAt = calcNewEyePos(-player.direction, 0, vec3(0), camDistance);
		*j = scale * normalize(upVector);
		*k = scale * normalize(vec3(-lookAt.x, 0, -lookAt.z));
		*i = scale * normalize(cross(*j, *k));
	}

	vec3 calcNewEyePos(float dx, float dy, vec3 mainPos, vec3 eyePos)
	{
		vec3 newEyePos;
		newEyePos.x = cos(dx) * (eyePos.x - mainPos.x) - sin(dx) * (eyePos.z - mainPos.z) + mainPos.x;
		newEyePos.y = eyePos.y + dy;
		newEyePos.z = sin(dx) * (eyePos.x - mainPos.x) + cos(dx) * (eyePos.z - mainPos.z) + mainPos.z;
		return newEyePos;
	}

	void moveKeyUpdate()
	{
		vec3 i, j, k;
		bool hasMoved = false;
		if (moveKeyAction[0] == GLFW_PRESS)
		{
			calcIJK(&i, &j, &k, SPEED, camDistances[0], upVector);
			player.move(k, entities, NUM_ENTITIES);
			hasMoved = true;
		}
		if (moveKeyAction[1] == GLFW_PRESS)
		{
			calcIJK(&i, &j, &k, SPEED, camDistances[0], upVector);
			player.move(i, entities, NUM_ENTITIES);
			hasMoved = true;
		}
		if (moveKeyAction[2] == GLFW_PRESS)
		{
			calcIJK(&i, &j, &k, SPEED, camDistances[0], upVector);
			player.move(-k, entities, NUM_ENTITIES);
			hasMoved = true;
		}
		if (moveKeyAction[3] == GLFW_PRESS)
		{
			calcIJK(&i, &j, &k, SPEED, camDistances[0], upVector);
			player.move(-i, entities, NUM_ENTITIES);
			hasMoved = true;
		}
		if (moveKeyAction[4] == GLFW_PRESS)
		{
			player.direction += SPEED;
		}
		if (moveKeyAction[5] == GLFW_PRESS)
		{
			player.direction -= SPEED;
		}

		// prevent player from walking off edge
		if (hasMoved) {
			player.location.x = fmin(fmax(player.location.x, -MAX_X / 2.0), MAX_X / 2.0);
			player.location.z = fmin(fmax(player.location.z, -MAX_X / 2.0), MAX_X / 2.0);
			player.timeFrame += 0.1;
		}
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			gMat = (gMat + 1) % 4;
			return;
		}
		else if (key == GLFW_KEY_V && action == GLFW_PRESS)
		{
			view++;
			view %= NUM_VIEWS;
			return;
		}
		else if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
			player.material++;
			player.material %= NUM_MATERIALS;
			return;
		}
		else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			addCobweb();
			return;
		}
		for (int i = 0; i < NUM_MOVEKEYS; ++i) {
			if (key == moveKeys[i]) {
				moveKeyAction[i] = action;
				return;
			}
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			drawMode = 1;
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			drawMode = 0;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		player.direction += (float)deltaX;
		dy = max(min((float)deltaY + dy, MAX_VIEW), -MAX_VIEW);
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
			Moving = true;
		}

		if (action == GLFW_RELEASE)
		{
			Moving = false;
			mouseDown = false;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char *data =
				stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				std::cout << "failed to load: " << (dir + faces[i]).c_str() << std::endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	}

	// Code to load in the three textures
	void initTex(const std::string& resourceDirectory)
	{
		for (int i = 0; i < NUMTEX; ++i) {
			textures[i] = make_shared<Texture>();
			textures[i]->setFilename(resourceDirectory + texFiles[i]);
			textures[i]->init();
			textures[i]->setUnit(0);
			textures[i]->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		}

		for (int i = 0; i < NUM_CUBE_TEX; ++i) {
			cubeTextures[i] = createSky(resourceDirectory, cubeFaceTextures[i]);
		}
	}

	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		dy = 0;
		resetPlayer();
		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		initProg(prog, resourceDirectory, "/simple_vert.glsl", "/simple_frag.glsl");
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");

		//initialize the textures we might use
		initTex(resourceDirectory);

		cubeProg = make_shared<Program>();
		initProg(cubeProg, resourceDirectory, "/cube_vert.glsl", "/cube_frag.glsl");
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");

		//create two frame buffer objects to toggle between
		glGenFramebuffers(2, frameBuf);
		glGenTextures(2, texBuf);
		glGenRenderbuffers(1, &depthBuf);
		createFBO(frameBuf[0], texBuf[0]);

		//set up depth necessary as rendering a mesh that needs depth test
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		//more FBO set up
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		//create another FBO so we can swap back and forth
		createFBO(frameBuf[1], texBuf[1]);
		//this one doesn't need depth

		//set up the shaders to blur the FBO just a placeholder pass thru now
		//next lab modify and possibly add other shaders to complete blur
		texProg = make_shared<Program>();
		initProg(texProg, resourceDirectory, "/pass_vert.glsl", "/tex_fragH.glsl");
		texProg->addUniform("texBuf");
		texProg->addUniform("dir");
		prog->addAttribute("vertPos");

		tex0Prog = make_shared<Program>();
		initProg(tex0Prog, resourceDirectory, "/tex_vert.glsl", "/tex_frag0.glsl");
		tex0Prog->addUniform("P");
		tex0Prog->addUniform("V");
		tex0Prog->addUniform("M");
		tex0Prog->addAttribute("vertPos");
		tex0Prog->addAttribute("vertNor");
		tex0Prog->addAttribute("vertTex");
		tex0Prog->addUniform("Texture0");

		tex1Prog = make_shared<Program>();
		initProg(tex1Prog, resourceDirectory, "/tex_vert.glsl", "/tex_frag1.glsl");
		tex1Prog->addUniform("P");
		tex1Prog->addUniform("V");
		tex1Prog->addUniform("M");
		tex1Prog->addAttribute("vertPos");
		tex1Prog->addAttribute("vertNor");
		tex1Prog->addAttribute("vertTex");
		tex1Prog->addUniform("Texture0");

		cubeProg = make_shared<Program>();
		initProg(cubeProg, resourceDirectory, "/cube_vert.glsl", "/cube_frag.glsl");
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");
	 }

	void initProg(shared_ptr<Program> prog, 
		const std::string& resourceDirectory, 
		const string vert, 
		const string frag)
	{
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + vert,
			resourceDirectory + frag);
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
	}

	void initGeom(const std::string& resourceDirectory)
	{
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;

		string errStr;

		sphere = make_shared<Shape>();
		sphere->loadMesh(resourceDirectory + "/sphere.obj");
		sphere->measure();
		sphere->resize();
		sphere->init();

		world = make_shared<Shape>();
		world->loadMesh(resourceDirectory + "/world.obj");
		world->measure();
		world->resize();
		world->init();

		/*flower = make_shared<Shape>();
		flower->loadMesh(resourceDirectory + "/Flower.obj");
		flower->measure();
		flower->resize();
		flower->init();*/

		// now read in the cube for the cubemap
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/cube.obj").c_str());

		cube = make_shared<Shape>();
		cube->createShape(TOshapes[0]);
		cube->measure();
		cube->init();

		//Initialize the geometry to render a quad to the screen
		initQuad();
		setRandomEntities(entities, vec3(20), vec3(-20), 20, 1);
	}

	/**** geometry set up for a quad *****/
	void initQuad()
	{
		//now set up a simple quad for rendering FBO
		glGenVertexArrays(1, &quad_VertexArrayID);
		glBindVertexArray(quad_VertexArrayID);

		static const GLfloat g_quad_vertex_buffer_data[] =
		{
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	}

	/* Helper function to create the framebuffer object and
		associated texture to write to */
	void createFBO(GLuint& fb, GLuint& tex)
	{
		//initialize FBO
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//set up framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//set up texture
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Error setting up frame buffer - exiting" << endl;
			exit(0);
		}
	}

	// To complete image processing on the specificed texture
	// Right now just draws large quad to the screen that is texture mapped
	// with the prior scene image - next lab we will process
	void ProcessImage(GLuint inTex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, inTex);

		// example applying of 'drawing' the FBO texture - change shaders
		texProg->bind();
		glUniform1i(texProg->getUniform("texBuf"), 0);
		glUniform2f(texProg->getUniform("dir"), -1, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		texProg->unbind();
	}

	void drawEntity(shared_ptr<MatrixStack> P, mat4 V, shared_ptr<MatrixStack> M, Entity e)
	{
		M->pushMatrix();
		M->translate(e.location);
		M->rotate(e.direction, YAXIS);
		M->scale(e.size);
		switch (e.type)
		{
		case TYPE_SPIDER:
			prog->bind();
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			SetMaterial(e.material);
			drawSpider(prog, M, e.timeFrame);
			prog->unbind();
			break;
		case TYPE_WATERMELON:
			tex1Prog->bind();
			glUniformMatrix4fv(tex1Prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(tex1Prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			drawWatermelon(tex1Prog, M);
			tex1Prog->unbind();
			break;
		case TYPE_CRATE:
			cubeProg->bind();
			glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			drawCrate(cubeProg, M, e.material);
			cubeProg->unbind();
			break;
		case TYPE_COBWEB:
			tex0Prog->bind();
			glUniformMatrix4fv(tex0Prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(tex0Prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(tex0Prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			drawCobweb(tex0Prog, M);
			tex0Prog->unbind();
			break;
		}
		M->popMatrix();
	}
	void drawSpider(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, float time)
	{
		float offset = 0.1;
		M->pushMatrix();
		M->translate(vec3(0, 0.15, 0));
		M->scale(0.1);
		M->pushMatrix(); // body
		M->translate(vec3(0, 0, 1));
		M->scale(vec3(1, 0.7, 1));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		sphere->draw(prog);
		M->popMatrix();
		float headRadius = 0.7;
		M->pushMatrix(); // head
		M->translate(vec3(0, 0, -0.2));
		drawSpiderEyes(prog, M, headRadius - 0.15, offset);
		M->scale(vec3(headRadius, 0.3, headRadius));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		sphere->draw(prog);
		M->popMatrix();
		drawSpiderLegs(prog, M, time);
		M->popMatrix();
	}

	void drawSpiderEyes(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, float headRadius, float offset)
	{
		float eyeSize = 0.025;
		float eyeOffset = eyeSize / 2;
		M->pushMatrix();
		M->translate(vec3(0, 0, -headRadius));
		for (int i = 0; i < 4; ++i) {
			float x = i * 2 * eyeSize - eyeOffset * 3;
			M->pushMatrix();
			M->translate(vec3(x, eyeSize + offset, 0));
			M->scale(vec3(eyeSize));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			sphere->draw(prog);
			M->popMatrix();
			M->pushMatrix();
			M->translate(vec3(x, -eyeSize + offset, -offset / 2));
			M->scale(vec3(eyeSize));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			sphere->draw(prog);
			M->popMatrix();
		}
		M->popMatrix();
	}

	void drawSpiderLegs(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, float time)
	{
		float adjustment = M_PI_4 / 2;
		float delta = -M_PI_4 / 2;
		vec3 scale = vec3(0.05, 0.05, 1);
		vec3 translate = vec3(-1, 0, 0);
		M->pushMatrix();
		for (int i = 0; i < 4; ++i) {
			float leg = (i % 2 - 0.5) * 2;
			float timeAdjust = time + delta * i;
			float distanceAdjust = 4.0;
			// float xRot = abs(cos(timeAdjust)) * leg / distanceAdjust;
			// float yRot = adjustment + i * delta + abs(sin(timeAdjust)) * leg / distanceAdjust;
			// float zRot = -adjustment - xRot;
			float xRot = 0;
			float yRot = (sin(timeAdjust) / 8 - adjustment * i) * leg / distanceAdjust;
			float zRot = -abs(cos(timeAdjust)) / distanceAdjust;
			drawSpiderLeg(prog, M, vec3(xRot, yRot, zRot), translate, scale);
			drawSpiderLeg(prog, M, vec3(xRot, -yRot, -zRot), -translate, scale);
		}
		M->popMatrix();
	}

	void drawSpiderLeg(shared_ptr<Program> prog, shared_ptr<MatrixStack> M,
		vec3 rotations, vec3 translate, vec3 scale)
	{
		M->pushMatrix();
		M->rotate(rotations.y, YAXIS);
		M->rotate(rotations.x, XAXIS);
		M->rotate(rotations.z, ZAXIS);
		M->translate(translate);
		M->pushMatrix();
		M->rotate(M_PI_2, YAXIS);
		M->rotate(M_PI_2, XAXIS);
		M->translate(vec3(0, 0.8 * translate.x, 0.8));
		M->scale(scale);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		sphere->draw(prog);
		M->popMatrix();
		M->rotate(M_PI_2, YAXIS);
		M->scale(scale);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		sphere->draw(prog);
		M->popMatrix();
	}


	void drawWatermelon(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
	{
		M->pushMatrix();
		M->scale(vec3(gDScale / 5, gDScale / 10, gDScale / 5));
		M->translate(vec3(0, (world->max - world->min).y / 2.0, 0));
		M->translate(-1.0f*gDTrans);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		textures[TEX_WATERMELON]->bind(prog->getUniform("Texture0"));
		world->draw(prog);
		M->popMatrix();
	}

	void drawCrate(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, int material)
	{
		M->pushMatrix();
		M->translate(vec3(0, (cube->max - cube->min).y / 2.0, 0));
		M->scale(gDScale);
		M->translate(-1.0f*gDTrans);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextures[material + WOOD_CRATE]);
		cube->draw(prog);
		M->popMatrix();
	}

	void drawCobweb(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
	{
		M->pushMatrix();
		M->scale(vec3(gDScale / 5, 0.001, gDScale / 5));
		M->translate(vec3(0, (world->max - world->min).y / 2.0 + 0.1, 0));
		M->translate(-1.0f*gDTrans);
		M->rotate(M_PI_2, ZAXIS);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		textures[TEX_COBWEB]->bind(prog->getUniform("Texture0"));
		world->draw(prog);
		M->popMatrix();
	}

	void drawSkybox(shared_ptr<Program> prog, shared_ptr<MatrixStack> P, mat4 V, shared_ptr<MatrixStack> M)
	{
		//draw the sky box
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glDepthFunc(GL_LEQUAL);
		M->pushMatrix();
		M->loadIdentity();
		M->translate(player.location);
		M->scale(50.0);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextures[SKYBOX]);
		cube->draw(prog);
		glDepthFunc(GL_LESS);
		M->popMatrix();
		prog->unbind();
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		if (Moving)
		{
			//set up to render to buffer
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[0]);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width/(float)height;
		moveKeyUpdate();
		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		vec3 eyePos = calcNewEyePos(-player.direction, dy, player.location, player.location + camDistances[view]);
		auto V = glm::lookAt(eyePos, player.location, upVector);
		auto M = make_shared<MatrixStack>();
		// Apply perspective projection.
		P->pushMatrix();
		P->perspective(45.0f, aspect, 0.01f, 500.0f);

		M->pushMatrix();
		M->loadIdentity();
		//Draw scene: player and then entities
		drawEntity(P, V, M, player);
		for (int i = 0; i < NUM_ENTITIES; ++i) {
			drawEntity(P, V, M, entities[i]);
		}
		for (int i = 0; i < cobwebs.size(); ++i) {
			drawEntity(P, V, M, *cobwebs.at(i));
		}

		if (dy > -0.1) // if camera is above ground, draw floor
		{
			cubeProg->bind();
			glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			M->pushMatrix();
			M->scale(vec3(MAX_X, 0.0, MAX_X));
			M->scale(gDScale);
			M->translate(-1.0f*gDTrans);
			glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextures[FLOOR]);
			cube->draw(cubeProg);
			M->popMatrix();
			cubeProg->unbind();
		}

		drawSkybox(cubeProg, P, V, M);

		M->popMatrix();

		P->popMatrix();

		if (Moving)
		{
			for (int i = 0; i < 3; i ++)
			{
				//set up framebuffer
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[(i+1)%2]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//set up texture
				ProcessImage(texBuf[i%2]);
			}

			/* now draw the actual output */
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ProcessImage(texBuf[1]);

			/*code to write out the FBO (texture) just once -an example*/
			if (FirstTime)
			{
				assert(GLTextureWriter::WriteImage(texBuf[0], "Texture_output.png"));
				FirstTime = false;
			}
		}

		for (int i = 0; i < NUM_ENTITIES; ++i)
		{
			entities[i].timeFrame += ANIMATION_SPEED;
		}
	}

	// helper function to set materials for shading
	void SetMaterial(int i) {
		switch (i) {
		case 0: //shiny blue plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
			glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
			glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
			glUniform1f(prog->getUniform("shine"), 120.0);
			break;
		case 1: // flat grey
			glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
			glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
			glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(prog->getUniform("shine"), 4.0);
			break;
		case 2: //brass
			glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
			glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
			glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.94117, 0.8078);
			glUniform1f(prog->getUniform("shine"), 27.9);
			break;
		case 3: //copper
			glUniform3f(prog->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
			glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.94117, 0.8078);
			glUniform1f(prog->getUniform("shine"), 27.9);
			break;
		case 4: //spider
			glUniform3f(prog->getUniform("MatAmb"), 0.0f, 0.0f, 0.0f);
			glUniform3f(prog->getUniform("MatDif"), 0.1f, 0.1f, 0.1f);
			glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(prog->getUniform("shine"), 4.0);
			break;
		case 5: // bright green
			glUniform3f(prog->getUniform("MatAmb"), 0.2, 0.4, 0.2);
			glUniform3f(prog->getUniform("MatDif"), 0.3, 0.8, 0.3);
			glUniform3f(prog->getUniform("MatSpec"), 0.4, 0.7, 0.4);
			glUniform1f(prog->getUniform("shine"), 1.0);
			break;
		case 6: // dark green
			glUniform3f(prog->getUniform("MatAmb"), 0.0, 0.1, 0.0);
			glUniform3f(prog->getUniform("MatDif"), 0.0, 0.3, 0.0);
			glUniform3f(prog->getUniform("MatSpec"), 0.0, 0.1, 0.0);
			glUniform1f(prog->getUniform("shine"), 1.0);
			break;
		}
	}

};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
			resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
			// Render scene.
			application->render();

			// Swap front and back buffers.
			glfwSwapBuffers(windowManager->getHandle());
			// Poll for and process events.
			glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
