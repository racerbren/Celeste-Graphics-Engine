#version 330

layout (location=0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 lightPos;

void main() {
    FragColor = texture(ourTexture, TexCoord);
}