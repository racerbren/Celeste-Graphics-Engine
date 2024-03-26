#include <iostream>
#include "Mesh3D.h"
#include <glad/glad.h>
#include <GL/GL.h>

using std::vector;
using sf::Color;
using sf::Vector2u;
using glm::mat4;
using glm::vec4;

Mesh3D::Mesh3D(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& faces,
	const sf::Image& texture) 
	: m_vertexCount(vertices.size()), m_faceCount(faces.size()) {

	// Generate a vertex array object on the GPU.
	glGenVertexArrays(1, &m_vao);
	// "Bind" the newly-generated vao, which makes future functions operate on that specific object.
	glBindVertexArray(m_vao);

	// Generate a vertex buffer object on the GPU.
	uint32_t vbo;
	glGenBuffers(1, &vbo);

	// "Bind" the newly-generated vbo, which makes future functions operate on that specific object.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// This vbo is now associated with m_vao.
	// Copy the contents of the vertices list to the buffer that lives on the GPU.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), &vertices[0], GL_STATIC_DRAW);
	
	// Inform OpenGL how to interpret the buffer. Each vertex now has TWO attributes; a position and a color.
	// Atrribute 0 is position: 3 contiguous floats (x/y/z)...
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
	glEnableVertexAttribArray(0);

	// Attribute 1 is texture (u,v): 2 contiguous floats, starting 12 bytes after the beginning of the vertex.
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex3D), (void*)12);
	glEnableVertexAttribArray(1);

	// Generate a second buffer, to store the indices of each triangle in the mesh.
	uint32_t ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(uint32_t), &faces[0], GL_STATIC_DRAW);

	// Unbind the vertex array, so no one else can accidentally mess with it.
	glBindVertexArray(0);


	// Generate a texture on the GPU.
	uint32_t texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize().x, texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_activeTexture = texID;
	m_textures.push_back(texID);
	m_textureIndex = 0;
}

void Mesh3D::render(sf::RenderWindow& window) {
	// Activate the mesh's vertex array.
	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, m_activeTexture);

	// Draw the vertex array, using its "element buffer" to identify the faces.
	glDrawElements(GL_TRIANGLES, m_faceCount, GL_UNSIGNED_INT, nullptr);
	// Deactivate the mesh's vertex array and texture.
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh3D Mesh3D::square(const sf::Image& texture) {
	return Mesh3D(
		{ 
		  { 0.5, 0.5, 0, 1, 0 },    // TR
		  { 0.5, -0.5, 0, 1, 1 },   // BR
		  { -0.5, -0.5, 0, 0, 1 },  // BL
		  { -0.5, 0.5, 0, 0, 0 },   // TL
		}, 
		{ 
			3, 1, 2,
			3, 1, 0,
		},
		texture
	);
}

Mesh3D Mesh3D::triangle(const sf::Image& texture) {
	return Mesh3D(
		{ { -0.5, -0.5, 0., 0., 1. },
		  { -0.5, 0.5, 0, 0., 0. },
		  { 0.5, 0.5, 0, 1, 0 } },
		{ 2, 1, 0 },
		texture
	);
}

Mesh3D Mesh3D::cube(const sf::Image& texture) {
	std::vector<Vertex3D> verts = {
		/*BUR*/{ 0.5, 0.5, -0.5,  0, 0},
		/*BUL*/{ -0.5, 0.5, -0.5, 0, 0},
		/*BLL*/{ -0.5, -0.5, -0.5, 1.0, 0 },
		/*BLR*/{ 0.5, -0.5, -0.5, 0, 1.0},
		/*FUR*/{ 0.5, 0.5, 0.5, 1.0, 0},
		/*FUL*/{-0.5, 0.5, 0.5, 1.0, 1.0},
		/*FLL*/{-0.5, -0.5, 0.5, 0, 1.0},
		/*FLR*/{0.5, -0.5, 0.5, 1.0, 1.0}
	};
	std::vector<uint32_t> tris = {
		0, 1, 2,
		0, 2, 3,
		4, 0, 3,
		4, 3, 7,
		5, 4, 7,
		5, 7, 6,
		1, 5, 6,
		1, 6, 2,
		4, 5, 1,
		4, 1, 0,
		2, 6, 7,
		2, 7, 3
	};

	return Mesh3D(verts, tris, texture);
}

void Mesh3D::addTexture(sf::Image texture)
{
	uint32_t textureID;

	// Generate a texture on the GPU.
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize().x, texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(textureID);
}

void Mesh3D::cycleTexture()
{
	if (m_textureIndex == m_textures.size() - 1)
		m_textureIndex = 0;
	else
		m_textureIndex++;
	m_activeTexture = m_textures[m_textureIndex];
}

