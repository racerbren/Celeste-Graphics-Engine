#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include "Shader.h"

class BillboardMesh {

private:
	std::vector<float> m_vertices; 
	std::vector<uint32_t> m_faces;
	std::vector<uint32_t> m_textureIDs;
	std::vector<SDL_Surface*> m_textures;
	uint32_t m_vertexCount, m_vao, m_vbo, m_ebo, m_activeTexture;

public:
	BillboardMesh(float width, float height, SDL_Surface* initialTexture);
	void render(Shader& shader);
};
