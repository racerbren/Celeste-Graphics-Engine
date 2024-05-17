#include <iostream>
#include "Mesh3D.h"
#include <glad/glad.h>
#include <GL/GL.h>

using std::vector;
using glm::mat4;
using glm::vec4;

Mesh3D::Mesh3D(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& faces, const std::vector<Map>& maps) 
{
	this->m_vertices = vertices;
	this->m_faces = faces;
	this->m_maps = maps;

	// Generate a vertex array object on the GPU.
	glGenVertexArrays(1, &m_vao);
	// "Bind" the newly-generated vao, which makes future functions operate on that specific object.
	glBindVertexArray(m_vao);

	// Generate a vertex buffer object on the GPU.
	glGenBuffers(1, &m_vbo);

	// "Bind" the newly-generated vbo, which makes future functions operate on that specific object.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// This vbo is now associated with m_vao.
	// Copy the contents of the vertices list to the buffer that lives on the GPU.
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3D), &m_vertices[0], GL_STATIC_DRAW);
	
	// Inform OpenGL how to interpret the buffer. Each vertex now has 3 attributes; a position, a normal, and a texture.
	// Atrribute 0 is position: 3 contiguous floats (x/y/z)...
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
	glEnableVertexAttribArray(0);

	//Attribute 1 is a normal: 3 contiguous floats
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex3D), (void*)12);
	glEnableVertexAttribArray(1);

	// Attribute 2 is texture (u,v): 2 contiguous floats, starting 12 bytes after the beginning of the vertex.
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex3D), (void*)24);
	glEnableVertexAttribArray(2);

	// Generate a second buffer, to store the indices of each triangle in the mesh.
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(uint32_t), &m_faces[0], GL_STATIC_DRAW);

	// Unbind the vertex array, so no one else can accidentally mess with it.
	glBindVertexArray(0);

	// Generate a texture on the GPU.
	Map diffuse = m_maps[0];
	glBindTexture(GL_TEXTURE_2D, diffuse.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int mode = GL_RGB;
	if (diffuse.texture->format->BytesPerPixel == 4)
		mode = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, mode, diffuse.texture->w, diffuse.texture->h, 0, mode, GL_UNSIGNED_BYTE, diffuse.texture->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_textureIndex = 0;
	m_activeTexture = diffuse.id;
}

void Mesh3D::render(Shader& shader, uint32_t shadowMapID) {
	// Activate the mesh's vertex array.
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_activeTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMapID);

	//Activate the mesh's shader
	shader.activate();

	// Draw the vertex array, using its "element buffer" to identify the faces.
	glDrawElements(GL_TRIANGLES, m_faces.size(), GL_UNSIGNED_INT, nullptr);

	//Disable the mesh's shader
	shader.disable();

	// Deactivate the mesh's vertex array and texture.
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh3D::addTexture(std::string path, std::string name)
{
	uint32_t textureID;

	SDL_Surface* texture = IMG_Load(path.c_str());

	//Generate new texture on the GPU
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int mode = GL_RGB;
	if (texture->format->BytesPerPixel == 4)
		mode = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, mode, texture->w, texture->h, 0, mode, GL_UNSIGNED_BYTE, texture->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	Map map;
	map.id = textureID;
	map.path = path;
	map.texture = texture;
	map.type = name;

	m_maps.push_back(map);
}

void Mesh3D::cycleTexture()
{
	if (m_textureIndex == m_maps.size() - 1)
		m_textureIndex = 0;
	else
		m_textureIndex++;
	m_activeTexture = m_maps[m_textureIndex].id;
}
