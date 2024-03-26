#include <glad/glad.h>
#include <SDL.h>
#include <iostream>

#include "Shader.h"

#undef main

//Set width and height of the window
#define width 1280
#define height 1060

//Define the vertices and faces of a rectangle
//float vertices[] = {
//	 0.5f,  0.5f, 0.0f, 
//	 0.5f, -0.5f, 0.0f,  
//	-0.5f, -0.5f, 0.0f,  
//	-0.5f,  0.5f, 0.0f
//};
//unsigned int faces[] = {
//	0, 1, 3,
//	1, 2, 3
//};

float vertices[] = {
	-0.5f,	-0.5f,	0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f,	 0.5f,	0.0f, 0.0f, 1.0f, 0.0f,
	 0.5f,	-0.5f,	0.0f, 0.0f, 0.0f, 1.0f
};
unsigned int faces[] = {
	0, 1, 2
};

int main(int argc, char* argv[])
{
	//Initialize SDL2 library and set OpenGL version 3.30
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	//Create a window and event handler for SDL2. The window is given a OpenGL flag for rendering with OpenGL context
	SDL_Window* window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	SDL_Event event;

	//Create an OpenGL context for SDL2
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	//Load all OpenGL functions using SDL loader
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Set up window coordinates for rendering
	glViewport(0, 0, width, height);

	//Draw in wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Do not draw faces if there is already something there
	glEnable(GL_DEPTH_TEST);

	//Declare objects
	unsigned int VAO, VBO, EBO;

	//Create a vertex array object on the GPU
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create one vertex buffer object on the GPU by sending the data from the CPU to the GPU
	//Bind the VBO to the buffer type of GL_ARRAY_BUFFER
	//Copy the vertex data into the current buffer's memory by setting the data and using it many times
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Create an element buffer array on the GPU, and bind the EBO by copying the faces into the buffer of type element array buffer
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	//Tell OpenGL how to interpret the vertex buffer data and store it in the current vertex array object
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader defaultShader;
	defaultShader.load("Shaders/default.vert", "Shaders/default.frag");
	defaultShader.activate();

	//main loop runs until window is closed
	bool destroyed = false;
	while (!destroyed)
	{
		//Check if window "x" was clicked
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				destroyed = true;
			}
		}

		//Clear the depth buffer bit
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (sizeof(faces) / sizeof(*faces)) + (sizeof(vertices) / sizeof(*vertices)), GL_UNSIGNED_INT, 0);
		//Disable the VAO
		glBindVertexArray(0);

		//Update the window with OpenGL rendering by swapping the back buffer with the front buffer.
		//The front buffer contains the final image to draw to the window while the back buffer renders everything.
		SDL_GL_SwapWindow(window);
	}

	//Deallocate memory for vertex array and buffer objects and the shader program
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//If the window is closed, clean up and exit SDL2
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}