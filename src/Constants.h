#pragma once


#define SPEED 0.1
#define NUM_ENTITIES 25
#define NUM_VIEWS 2
#define NUM_MOVEKEYS 6
#define ANIMATION_SPEED 0.1;
#define MAX_X 50

#define TYPE_SPIDER 0
#define TYPE_WATERMELON 1
#define TYPE_CRATE 2
#define TYPE_COBWEB 3
#define NUM_TYPES 3

#define MAT_BLUE_PLASTIC 0
#define MAT_FLAT_GREY 1
#define MAT_BRASS 2
#define MAT_COPPER 3
#define MAT_SPIDER 4
#define MAT_BRIGHT_GREEN 5
#define MAT_FLAT_GREEN 6
#define NUM_MATERIALS 7

const vec3 XAXIS = vec3(1.0, 0.0, 0.0);
const vec3 YAXIS = vec3(0.0, 1.0, 0.0);
const vec3 ZAXIS = vec3(0.0, 0.0, 1.0);
const float MAX_VIEW = 80.0 / 180 * M_PI;
const int NUM_MAT[NUM_TYPES] = { NUM_MATERIALS, 1, 2 };

const vec3 defaultWatermelon = vec3(0.2, 0.1, 0.2);
const vec3 camDistances[NUM_VIEWS] = { vec3(0, 0.2, 1), vec3(0, 0.2, -1) };

#define TEX_WATERMELON 0
#define TEX_COBWEB 1
#define TEX_FLOWER 2
#define NUMTEX 3
const string texFiles[NUMTEX] = { "/grass.jpg", "/cobweb.jpg ", "/Flower_0.jpg" };

#define SKYBOX 0
#define FLOOR 1
#define WOOD_CRATE 2
#define METAL_CRATE 3
#define NUM_CUBE_TEX 4
vector<std::string> cubeTextureFaces[NUM_CUBE_TEX] = {
	vector<std::string> {
		"/hw_morning/morning1_rt.tga",
		"/hw_morning/morning1_lf.tga",
		"/hw_morning/morning1_up.tga",
		"/hw_morning/morning1_dn.tga",
		"/hw_morning/morning1_ft.tga",
		"/hw_morning/morning1_bk.tga"
	},
	vector<std::string> {
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg"
	},
	vector<std::string> {
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg"
	},
	vector<std::string> {
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg"
	}
};