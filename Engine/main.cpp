#include <glad/glad.h>
#include <SDL.h>
#include <iostream>

#undef main

//Set width and height of the window
#define width 1280
#define height 1060

//Temporary vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos; // the position variable has attribute position 0\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 vertexColor; // specify a color output to the fragment shader\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor\n"
"	vertexColor = aColor; // set the output variable to a dark-red color\n"
"}\0";

//Temporary fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(vertexColor, 1.0f);\n"
"}\0";

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

	//Create a vertex shader on the GPU
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Attach the shader source code, in this case the c-string at the top of our code, to the vertexShader we created in VRAM
	//Then, compile the shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Check if the compile status of the vertex shader is successful or not and record it in success
	int  success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//Report the info given by the check above
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Create, attach, compile the fragment shader. Same as before, but for the fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//Report the info given by the check above
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Create a shader program through OpenGL
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	//Attach the vertex and fragment shaders to the created shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//Link the attached shaders through the program
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	//Clean up the shader objects since we do not need them anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Tell OpenGL how to interpret the vertex buffer data and store it in the current vertex array object
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

		//Activate the shader program, bind the vertex array object to the current buffer type, and draw the triangle
		glUseProgram(shaderProgram);
		/*float timeValue = SDL_GetTicks() / 1000.0f;
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

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
	glDeleteProgram(shaderProgram);

	//If the window is closed, clean up and exit SDL2
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}