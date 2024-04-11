#version 330
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

void main() {
    // Project the position to clip space.
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
    Normal = vNormal;
    TexCoord = vTexCoord;
    FragPos = vec3(model * vec4(vPosition, 1.0));
}