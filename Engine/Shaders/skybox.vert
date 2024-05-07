#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	//Set incoming position vectors of the cube vertices to be the texture coordinates for interpolated use in the fragment shader
	TexCoords = aPos;
	gl_Position = projection * view * vec4(aPos, 1.0);
}
