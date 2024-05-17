#include "BillboardMesh.h"

BillboardMesh::BillboardMesh(float width, float height, SDL_Surface* initialTexture)
{
	m_vertices = { {
			width / 2.0f,  height / 2.0f, 0.0f, 1.0f, 1.0f,	  //TR
			width / 2.0f, -height / 2.0f, 0.0f, 1.0f, 0.0f,   //BR
			-width / 2.0f, -height / 2.0f, 0.0f, 0.0f, 0.0f,  //BL
			-width / 2.0f,  height / 2.0f, 0.0f, 0.0f, 1.0f   //TL
	} };

	m_faces = { {
			3, 1, 2,
			3, 1, 0
	} };

	m_vertexCount = 4;
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//Vertex Position
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//Vertex UV
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float), (void*)12);
	glEnableVertexAttribArray(2);

	//Faces to draw
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(uint32_t), &m_faces[0], GL_STATIC_DRAW);

	uint32_t texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, initialTexture->w, initialTexture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialTexture->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_activeTexture = texID;
	m_textureIDs.push_back(texID);
	m_textures.push_back(initialTexture);

	// Unbind the vertex array, so no one else can accidentally mess with it.
	glBindVertexArray(0);
}

void BillboardMesh::render(Shader& shader)
{
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_activeTexture);

	shader.activate();
	glDrawElements(GL_TRIANGLES, m_faces.size(), GL_UNSIGNED_INT, nullptr);
	shader.disable();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
