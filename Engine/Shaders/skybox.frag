#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	//Sample textures from the cubemap by interpolating the position vector to be used as a direction vector
	FragColor = texture(skybox, TexCoords);
}
