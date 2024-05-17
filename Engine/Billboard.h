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
#include "BillboardMesh.h"
#include "Shader.h"

class Billboard {

private:
	glm::vec3 m_position;
	glm::mat4 m_model;
	std::shared_ptr<BillboardMesh> m_mesh;

public:
	Billboard(std::shared_ptr<BillboardMesh>&& mesh, glm::vec3 position);
	void update(glm::vec3 cameraPosition);
	void render(Shader& shader);
};
