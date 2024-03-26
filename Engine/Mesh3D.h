#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Vertex3D {
	float_t x;
	float_t y;
	float_t z;

	float_t u;
	float_t v;

	Vertex3D(float_t a, float_t b, float_t c, float_t tu, float_t tv) 
		: x(a), y(b), z(c), u(tu), v(tv) {}
};

class Mesh3D {
private:
	uint32_t m_vao;
	std::vector<uint32_t> m_textures;
	uint32_t m_activeTexture;
	int m_textureIndex = 0;
	size_t m_vertexCount;
	size_t m_faceCount;

public:
	Mesh3D() = delete;

	
	/**
	 * @brief Construcst a Mesh3D using existing vectors of vertices and faces.
	*/
	Mesh3D(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& faces, 
		const sf::Image& texture);

	/**
	 * @brief Constructs a 1x1 square centered at the origin in world space.
	*/
	static Mesh3D square(const sf::Image& texture);
	/**
	 * @brief Constructs a 1x1x1 cube centered at the origin in world space.
	*/
	static Mesh3D cube(const sf::Image& texture);
	/**
	 * @brief Constructs the upper-left half of the 1x1 square centered at the origin.
	*/
	static Mesh3D triangle(const sf::Image& texture);

	/**
	 * @brief Renders the mesh to the given context.
	 */
	void render(sf::RenderWindow& window);
	
	void addTexture(sf::Image texture);
	void cycleTexture();
};
