#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "Shader.h"
#include "Object3D.h"
#include "Mesh3D.h"
#include "AssimpImport.h"

#undef main

//Set width and height of the window and create a window. The window is given a OpenGL flag for rendering with OpenGL context
int width = 1080;
int height = 720;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = width / 2;
float lastY = height / 2;
float yaw = -90.0f;
float pitch = 0.0f;

void mouseCallback(SDL_Window* window, double x, double y, SDL_Event event)
{
	float sensitivity = 0.1f;

	yaw += sensitivity * event.motion.xrel;
	pitch -= sensitivity * event.motion.yrel;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

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
	if (event.type == SDL_MOUSEMOTION)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		mouseCallback(window, static_cast<double>(x), static_cast<double>(y), event);
	}
}

void init()
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
	
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

int main(int argc, char* argv[])
{
	init();

	SDL_Window* window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	//Create an OpenGL context for SDL2
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	//Load all OpenGL functions using SDL loader
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	//Set up window coordinates for rendering
	glViewport(0, 0, width, height);

	//Do not draw faces if there is already something there
	glEnable(GL_DEPTH_TEST);

	//Enable MSAA
	glEnable(GL_MULTISAMPLE);

	//Create an event handler for SDL2
	SDL_Event event;

	auto skull = assimpLoad("resources/12140_Skull_v3_L2.obj", true, false, false);
	skull.move(glm::vec3(0, -15, -40));
	skull.rotate(glm::vec3(-90, 0, 0));

	auto bunny = assimpLoad("resources/bunny_textured.obj", true, false, false);
	bunny.move(glm::vec3(0.2, -1, -5));
	bunny.grow(glm::vec3(9, 9, -9));

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
		skull.render(defaultShader);
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