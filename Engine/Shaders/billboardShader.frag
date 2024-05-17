#version 330

layout (location=0) out vec4 FragColor;

in vec2 TexCoord;
  
uniform sampler2D tex;
uniform vec3 viewPos;

void main()
{
    //Compute texture color of fragment
    FragColor = texture(tex, TexCoord);
}
