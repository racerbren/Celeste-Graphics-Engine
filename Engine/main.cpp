#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "Shader.h"
#include "Object3D.h"
#include "Mesh3D.h"
#include "AssimpImport.h"

#undef main

//Set width and height of the window
int width = 1080;
int height = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(SDL_Window* window, SDL_Event event)
{
	float cameraSpeed = 2.5f * deltaTime;
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == 119) //W
			cameraPos += cameraSpeed * cameraFront;
		if (event.key.keysym.sym == 97)  //A
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (event.key.keysym.sym == 115) //S
			cameraPos -= cameraSpeed * cameraFront;
		if (event.key.keysym.sym == 100) //D
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

}

int main(int argc, char* argv[])
{
	//Initialize SDL2 library and set OpenGL version 3.30
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	//MSAA
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

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

	//Do not draw faces if there is already something there
	glEnable(GL_DEPTH_TEST);
	//Enable MSAA
	glEnable(GL_MULTISAMPLE);

	auto bunny = assimpLoad("resources/12140_Skull_v3_L2.obj", true, false, false);
	bunny.move(glm::vec3(0, -15, -40));
	bunny.rotate(glm::vec3(-90, 0, 0));
	//bunny.grow(glm::vec3(9, 9, -9));

	Shader defaultShader;
	defaultShader.load("Shaders/default.vert", "Shaders/default.frag");
	defaultShader.activate();
	
	int* wide = &width;
	int* tall = &height;
	SDL_GetWindowSize(window, wide, tall);

	glm::mat4 camera = glm::lookAt(cameraPos, cameraFront, cameraUp);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0), static_cast<double>(*wide) / *tall, 0.1, 100.0);
	defaultShader.setUniform("view", camera);
	defaultShader.setUniform("projection", perspective);

	//main loop runs until window is closed
	bool destroyed = false;
	while (!destroyed)
	{
		float currentFrame = SDL_GetTicks() / 1000.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Check if window "x" was clicked
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				destroyed = true;
			}
			processInput(window, event);
		}
		camera = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		defaultShader.setUniform("view", camera);

		//Clear the depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render Here
		bunny.render(defaultShader);
		//bunny.rotate(glm::vec3(0, 0.003, 0));

		//Update the window with OpenGL rendering by swapping the back buffer with the front buffer.
		//The front buffer contains the final image to draw to the window while the back buffer renders everything.
		SDL_GL_SwapWindow(window);
	}

	//If the window is closed, clean up and exit SDL2
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}