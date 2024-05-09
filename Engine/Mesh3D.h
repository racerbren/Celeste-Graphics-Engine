#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

#include "Shader.h"

struct Vertex3D {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	Vertex3D(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex)
		: position(pos), normal(norm), texCoords(tex) {}
};

struct Map {
	uint32_t id;
	SDL_Surface* texture;
	std::string type;
	std::string path;
};

class Mesh3D {
private:
	uint32_t m_vao;
	uint32_t m_vbo;
	uint32_t m_ebo;

public:
	std::vector<Vertex3D> m_vertices;
	std::vector<uint32_t> m_faces;
	std::vector<Map> m_maps;

	Mesh3D() = delete;

	/**
	 * @brief Construcst a Mesh3D using existing vectors of vertices and faces.
	*/
	Mesh3D(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& faces, const std::vector<Map>& maps);

	/**
	 * @brief Renders the mesh to the given context.
	 */
	void render(Shader& shader, uint32_t shadowMapID);

};
