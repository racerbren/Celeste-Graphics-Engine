#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

#include "Shader.h"

class Skybox {
private:
	uint32_t m_vao;
	uint32_t m_vbo;
	uint32_t m_id;
public:
	Skybox(std::vector<std::string> faces);
	uint32_t loadSkyBox(std::vector<std::string> faces);
	void render(Shader& shader);
};