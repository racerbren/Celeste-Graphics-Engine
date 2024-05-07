#include "Skybox.h"

std::vector<glm::vec3> skyboxVertices = {
    // positions          
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),

    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),

    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),

    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),

    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),

    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f)
};

Skybox::Skybox(std::vector<std::string> faces)
{
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
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(glm::vec3), &skyboxVertices[0], GL_STATIC_DRAW);

    // Atrribute 0 is position: 3 contiguous floats (x/y/z)...
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(0);

    //Disable vertex array object
    glBindVertexArray(0);

    m_id = loadSkyBox(faces);
}

uint32_t Skybox::loadSkyBox(std::vector<std::string> faces)
{
	//Generate and bind skybox to gl cube map as you would any other texture
	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, mode;
	SDL_Surface* image;
	for (int i = 0; i < faces.size(); i++)
	{
		//Load the texture image
		image = IMG_Load(faces[i].c_str());

		//Determine the mode for the texture image by its format
		mode = GL_RGB;
		if (image->format->BytesPerPixel == 4)
			mode = GL_RGBA;

		//Determine the width and height of the texture image
		width = image->w;
		height = image->h;

		//Generate the texture image for the current cube map texture object
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
	}

	//Specify texture wrapping for cube map
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return id;
}

void Skybox::render(Shader& shader)
{
    glDepthMask(GL_FALSE);
    shader.activate();
    glBindVertexArray(m_vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}