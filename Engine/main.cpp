#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "Shader.h"
#include "Object3D.h"
#include "Mesh3D.h"
#include "AssimpImport.h"
#include "Animator.h"
#include "Skybox.h"
//#include "Billboard.h"
//#include "BillboardMesh.h"

#undef main

//Window width and height
int width = 1080;
int height = 720;

//Keep track of scene origin
glm::vec3 origin = glm::vec3(0.0f);

glm::vec3 cameraPos = origin;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Positions of dirt mounds
glm::vec3 mound1pos = glm::vec3(-2, -3.15, -7);
glm::vec3 mound2pos = glm::vec3(9, -3.15, 1);
glm::vec3 mound3pos = glm::vec3(-3, -3.15, 8);
glm::vec3 mound4pos = glm::vec3(-7, -3.15, 0);
glm::vec3 mound5pos = glm::vec3(6.5, -3.15, -6.5);
std::vector<glm::vec3> moundPositions = { mound1pos, mound2pos, mound3pos, mound4pos, mound5pos };

bool renderMound1 = true;
bool renderMound2 = true;
bool renderMound3 = true;
bool renderMound4 = true;
bool renderMound5 = true;
std::vector<bool> moundBools = { renderMound1, renderMound2, renderMound3, renderMound4, renderMound5 };

//Directional light
glm::vec3 sun = glm::vec3(-8.0f, 6.0f, -1.0f);

//Point light
glm::vec3 fire = glm::vec3(3.85, -2.57, -1.91);

//Set the resolution of the shadow map
const uint32_t shadowWidth = 2048, shadowHeight = 2048;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = width / 2;
float lastY = height / 2;
float yaw = -90.0f;
float pitch = 0.0f;

SDL_Surface* windowIcon = IMG_Load("resources/skull_icon.png");

void createShadowMap(uint32_t& fbo, uint32_t& id)
{
	//Create frame buffer object to represent shadow map
	glGenFramebuffers(1, &fbo);

	//Create a texture for the shadow map to be drawn to
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	//Set texture wrapping for depth map to clamp to a white border so that any objects outside of the light's frustum will not be in shadow
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Attach the shadow map to the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);

	//Tell OpenGL not to render any color buffer and bind the frame buffer to 0
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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
	float cameraSpeed = 3.5f * deltaTime;
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
		if (event.key.keysym.sym == 101) //E for interacting with mounds
		{
			int i = 0;
			for (auto mound : moundPositions)
			{
				float distance = glm::length(cameraPos - mound);
				if (distance <= 5)
				{
					moundBools[i] = false;
				}
				i++;
			}
		}
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
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

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
	//Set width and height of the window and create a window. The window is given a OpenGL flag for rendering with OpenGL context
	SDL_Window* window = SDL_CreateWindow("Pirate Island", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	//Create an OpenGL context for SDL2
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	SDL_SetWindowIcon(window, windowIcon);

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

	//Reference the skybox images
	std::vector<std::string> faces =
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	//Load the skybox and hold onto the id associated with the texture image
	Skybox defaultSkybox(faces);

	auto island = assimpLoad("resources/island/island.obj", true, false, false);
	island.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	island.move(glm::vec3(0, -3, 0));

	auto fish = assimpLoad("resources/fish/12265_Fish_v1_L2.obj", true, false, false);
	fish.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	fish.move(glm::vec3(-2, -4, -7));
	fish.grow(glm::vec3(0.0125));

	auto wine = assimpLoad("resources/wine/14042_750_mL_Wine_Bottle_r_v1_L3.obj", true, false, false);
	wine.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	wine.move(glm::vec3(9, -4, 1));
	wine.grow(glm::vec3(0.025));

	auto slr = assimpLoad("resources/slrCamera/10124_SLR_Camera_SG_V1_Iteration2.obj", true, false, false);
	slr.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	slr.move(glm::vec3(-3, -4, 8));
	slr.grow(glm::vec3(0.003));
	slr.rotate(glm::vec3(-1.57, 0, 0));

	auto skull = assimpLoad("resources/skull/12140_Skull_v3_L2.obj", true, false, false);
	skull.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	skull.move(glm::vec3(-7, -4, 0));
	skull.grow(glm::vec3(0.0125));
	skull.rotate(glm::vec3(-90, 0, 0));

	auto goldenBunny = assimpLoad("resources/bunny/bunny_textured.obj", true, false, false);
	goldenBunny.setMaterial(glm::vec4(0.3, 1.0, 1.0, 32));
	goldenBunny.addTex("resources/gold.png", "diffuse");
	goldenBunny.cycleTex();
	goldenBunny.move(glm::vec3(6.5, -4, -6.5));
	goldenBunny.grow(glm::vec3(3));

	auto mound1 = assimpLoad("resources/mound/mound.obj", true, false, false);
	mound1.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	mound1.move(mound1pos);

	auto mound2 = assimpLoad("resources/mound/mound.obj", true, false, false);
	mound2.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	mound2.move(mound2pos);

	auto mound3 = assimpLoad("resources/mound/mound.obj", true, false, false);
	mound3.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	mound3.move(mound3pos);

	auto mound4 = assimpLoad("resources/mound/mound.obj", true, false, false);
	mound4.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	mound4.move(mound4pos);

	auto mound5 = assimpLoad("resources/mound/mound.obj", true, false, false);
	mound5.setMaterial(glm::vec4(0.3, 0.8, 0.1, 1));
	mound5.move(mound5pos);

	std::vector<Object3D> scene;
	scene.push_back(island);
	scene.push_back(std::move(fish));
	scene.push_back(std::move(wine));
	scene.push_back(std::move(slr));
	scene.push_back(std::move(skull));
	scene.push_back(std::move(goldenBunny));
	scene.push_back(mound1);
	scene.push_back(mound2);
	scene.push_back(mound3);
	scene.push_back(mound4);
	scene.push_back(mound5);

	//Create the shadow map
	uint32_t shadowMapFBO, shadowMapID;
	createShadowMap(shadowMapFBO, shadowMapID);

	Shader defaultShader;
	defaultShader.load("Shaders/default.vert", "Shaders/default.frag");
	defaultShader.activate();
	defaultShader.setUniform("ourTexture", 0);
	defaultShader.setUniform("shadowMap", 1);

	Shader skyboxShader;
	skyboxShader.load("Shaders/skybox.vert", "Shaders/skybox.frag");

	Shader simpleDepthShader;
	simpleDepthShader.load("Shaders/depthShader.vert", "Shaders/depthShader.frag");
	
	//Get the size of the window for setting the perspective matrix
	int* wide = &width;
	int* tall = &height;
	SDL_GetWindowSize(window, wide, tall);

	//Set up view and projection matrices for vertex shader
	glm::mat4 camera = glm::lookAt(cameraPos, cameraFront, cameraUp);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0), static_cast<double>(*wide) / *tall, 0.1, 100.0);
	defaultShader.setUniform("view", camera);
	defaultShader.setUniform("projection", perspective);
	defaultShader.setUniform("viewPos", cameraPos);
	//Skybox shader
	skyboxShader.setUniform("view", camera);
	skyboxShader.setUniform("projection", perspective);

	//Set directional light
	defaultShader.setUniform("dirLight.direction", -sun);

	//Set point light
	defaultShader.setUniform("pointLights[0].position", fire);
	defaultShader.setUniform("pointLights[0].linear", 0.7f);
	defaultShader.setUniform("pointLights[0].quadratic", 1.8f);

	//Model directional light source with parallel light rays
	glm::mat4 lightProj, lightView, lightSpace;
	float nearPlane = 1.0f, farPlane = 20.0f;
	lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	lightView = glm::lookAt(sun, origin, glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpace = lightProj * lightView;

	Animator fishAnimator;
	fishAnimator.addAnimation(std::make_unique<TranslationAnimation>(scene[1], 1.5, glm::vec3(0, 2, 0)));
	
	Animator wineAnimator;
	wineAnimator.addAnimation(std::make_unique<TranslationAnimation>(scene[2], 1.5, glm::vec3(0, 2, 0)));

	Animator slrAnimator;
	slrAnimator.addAnimation(std::make_unique<TranslationAnimation>(scene[3], 1.5, glm::vec3(0, 2, 0)));

	Animator skullAnimator;
	skullAnimator.addAnimation(std::make_unique<TranslationAnimation>(scene[4], 1.5, glm::vec3(0, 2, 0)));

	Animator goldenBunnyAnimator;
	goldenBunnyAnimator.addAnimation(std::make_unique<TRAnimation>(scene[5], 1.5, glm::vec3(0, 2, 0), glm::vec3(0, 6.28, 0)));
	goldenBunnyAnimator.addAnimation(std::make_unique<RotationAnimation>(scene[5], 3.5, glm::vec3(0, 12.56, 0)));

	fishAnimator.start();
	wineAnimator.start();
	slrAnimator.start();
	skullAnimator.start();
	goldenBunnyAnimator.start();

	std::vector<Animator> animators;
	animators.push_back(std::move(fishAnimator));
	animators.push_back(std::move(wineAnimator));
	animators.push_back(std::move(slrAnimator));
	animators.push_back(std::move(skullAnimator));
	animators.push_back(std::move(goldenBunnyAnimator));

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
		defaultShader.setUniform("viewPos", cameraPos);

		//Clear the depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render the scene to a depth map
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT); //Enable front face culling for rendering the depth map to avoid Peter Panning shadows

		int i = 0;
		for (auto obj : scene)
		{
			if (i > 5 && moundBools[i - 6] == false)
				i++;
			else
			{
				simpleDepthShader.activate();
				simpleDepthShader.setUniform("lightSpaceMatrix", lightSpace);
				obj.render(simpleDepthShader, 0);
				simpleDepthShader.disable();
				i++;
			}
		}
		
		glCullFace(GL_BACK);  //Re-enable backface culling
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Reset the viewport
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		i = 0;
		for (auto& animator : animators)
		{
			if (moundBools[i] == false)
				animator.tick(deltaTime);
			i++;
		}

		//Render scene objects with default shading
		i = 0;
		for (auto& obj : scene)
		{
			if (i > 5 && moundBools[i - 6] == false)
				i++;
			else
			{
				defaultShader.activate();
				defaultShader.setUniform("view", camera);
				defaultShader.setUniform("projection", perspective);
				defaultShader.setUniform("viewPos", cameraPos);
				defaultShader.setUniform("dirLight.direction", -sun);
				defaultShader.setUniform("pointLights[0].position", glm::vec3(3.85, -2.57, -1.91));
				defaultShader.setUniform("pointLights[0].linear", 0.7f);
				defaultShader.setUniform("pointLights[0].quadratic", 1.8f);
				defaultShader.setUniform("lightSpaceMatrix", lightSpace);
				obj.render(defaultShader, shadowMapID);
				defaultShader.disable();
				i++;
			}
		}

		//Render skybox last so fragments behind other objects are not rendered
		//Change depth function because depth buffer will be filled with 1.0 for the skybox and we want to check if the depth values equal the skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.activate();
		glm::mat4 view = glm::mat4(glm::mat3(camera));
		skyboxShader.setUniform("view", view);
		skyboxShader.setUniform("projection", perspective);
		defaultSkybox.render(skyboxShader);
		skyboxShader.disable();
		////Set the depth function back to default
		glDepthFunc(GL_LESS);

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