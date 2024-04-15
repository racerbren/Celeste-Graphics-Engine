#version 330

layout (location=0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
  
uniform sampler2D ourTexture;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

//(ambient, diffuse, specular, shininess)
uniform vec4 material;

void main() 
{
    //Normalize vectors for computing lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    //Compute lighting intensities
    float lambert = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.w);

    //Compute lighting colors
    vec3 ambient = material.x * lightColor;
    vec3 diffuse = (material.y * lambert) * lightColor;
    vec3 specular = (material.z * spec) * lightColor;

    //Compute texture color of fragment and Phong lighting combination
    vec4 texColor = texture(ourTexture, TexCoord);
    vec4 lightRes = vec4((ambient + diffuse + specular), 1.0);

    //Ouput the resulting fragment color
    FragColor = lightRes * texColor;
}