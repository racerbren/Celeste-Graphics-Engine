#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "Shader.h"

#undef main

//Set width and height of the window
#define width 1080
#define height 720

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

		//Render Here

		//Update the window with OpenGL rendering by swapping the back buffer with the front buffer.
		//The front buffer contains the final image to draw to the window while the back buffer renders everything.
		SDL_GL_SwapWindow(window);
	}

	//If the window is closed, clean up and exit SDL2
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}