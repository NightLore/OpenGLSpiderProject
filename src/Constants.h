#pragma once

constexpr float SPEED = 0.1f;
constexpr int NUM_ENTITIES = 25;
constexpr int NUM_VIEWS = 2;
constexpr int NUM_MOVEKEYS = 6;
constexpr float ANIMATION_SPEED = 0.1f;
constexpr float MAX_X = 50;

constexpr int TYPE_SPIDER = 0;
constexpr int TYPE_WATERMELON = 1;
constexpr int TYPE_CRATE = 2;
constexpr int TYPE_COBWEB = 3;
constexpr int NUM_TYPES = 3;

constexpr int MAT_BLUE_PLASTIC = 0;
constexpr int MAT_FLAT_GREY = 1;
constexpr int MAT_BRASS = 2;
constexpr int MAT_COPPER = 3;
constexpr int MAT_SPIDER = 4;
constexpr int MAT_BRIGHT_GREEN = 5;
constexpr int MAT_FLAT_GREEN = 6;
constexpr int NUM_MATERIALS = 7;

constexpr glm::vec3 XAXIS = glm::vec3(1.0, 0.0, 0.0);
constexpr glm::vec3 YAXIS = glm::vec3(0.0, 1.0, 0.0);
constexpr glm::vec3 ZAXIS = glm::vec3(0.0, 0.0, 1.0);
constexpr float MAX_VIEW = 80.0 / 180 * M_PI;
constexpr int NUM_MAT[NUM_TYPES] = { NUM_MATERIALS, 1, 2 };

constexpr glm::vec3 defaultWatermelon = glm::vec3(0.2, 0.1, 0.2);
constexpr glm::vec3 camDistances[NUM_VIEWS] = { glm::vec3(0, 0.2, 1), glm::vec3(0, 0.2, -1) };

constexpr int TEX_WATERMELON = 0;
constexpr int TEX_COBWEB = 1;
constexpr int TEX_FLOWER = 2;
constexpr int NUMTEX = 3;
const std::string texFiles[NUMTEX] = { "/grass.jpg", "/cobweb.jpg ", "/Flower_0.jpg" };

constexpr int SKYBOX = 0;
constexpr int FLOOR = 1;
constexpr int WOOD_CRATE = 2;
constexpr int METAL_CRATE = 3;
constexpr int NUM_CUBE_TEX = 4;
std::vector<std::string> cubeFaceTextures[NUM_CUBE_TEX] = {
	std::vector<std::string> {
		"/hw_morning/morning1_rt.tga",
		"/hw_morning/morning1_lf.tga",
		"/hw_morning/morning1_up.tga",
		"/hw_morning/morning1_dn.tga",
		"/hw_morning/morning1_ft.tga",
		"/hw_morning/morning1_bk.tga"
	},
	std::vector<std::string> {
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg",
		"/cube/wood.jpg"
	},
	std::vector<std::string> {
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg",
		"/cube/crate.jpg"
	},
	std::vector<std::string> {
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg",
		"/cube/metalcrate.jpg"
	}
};