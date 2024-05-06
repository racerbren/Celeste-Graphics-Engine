#version 330

layout (location=0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
  
uniform sampler2D ourTexture;
uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;
};
uniform DirectionalLight dirLight;

struct PointLight
{
    vec3 position;

    float linear;
    float quadratic;
};
const int numPointLights = 1;
uniform PointLight pointLights[numPointLights];

//(ambient x, diffuse y , specular z, shininess w)
uniform vec4 material;

vec4 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec4 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);

void main() 
{
    //Normalize vectors for computing lighting
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Compute texture color of fragment
    vec4 texColor = texture(ourTexture, TexCoord);

    //Calculate Phong Lighting
    vec4 lightRes = calculateDirectionalLight(dirLight, norm, viewDir);
    for (int i = 0; i < numPointLights; i++)
        lightRes += calculatePointLight(pointLights[i], norm, FragPos, viewDir);

    //Ouput the resulting fragment color
    FragColor = lightRes * texColor;
}

vec4 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    //Normalize direction vector from frag towards light
    vec3 lightDirection = normalize(-light.direction);

    //Calculate diffuse impact of lighting
    float diff = max(dot(normal, lightDirection), 0.0);

    //Calculate direction of the reflecting ray
    vec3 reflectDirection = reflect(-lightDirection, normal);
    //Calculate the specular component of lighting
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.w);

    //Combine Phong Lighting for the directional light
    vec3 ambient = vec3(1.0, 1.0, 1.0) * material.x;
    vec3 diffuse = vec3(1.0, 1.0, 1.0) * material.y * diff;
    vec3 specular = vec3(1.0, 1.0, 1.0) * material.z * spec;
    
    //Return effects of directional light
    return vec4((ambient + diffuse + specular), 1.0);
}

vec4 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    //Normalize vector from fragment to light source
    vec3 lightDirection = normalize(light.position - fragPos);

    //Calculate diffuse impact
    float diff = max(dot(normal, lightDirection), 0.0);

    //Calculate diretion of reflecting ray
    vec3 reflectDirection = reflect(-lightDirection, normal);
    //Calculate specular component
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.w);

    //Calculate attenuation of lighting, the reduction in light intensity as the distance of the light source increases
    float lightDistance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0f + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    //Combine Phone Lighting for point light
    vec3 ambient = vec3(1.0, 1.0, 1.0) * material.x * attenuation;
    vec3 diffuse = vec3(1.0, 1.0, 1.0) * material.y * diff * attenuation;
    vec3 specular = vec3(1.0, 1.0, 1.0) * material.z * spec * attenuation;

    //Return effects of point light
    return vec4((ambient + diffuse + specular), 1.0);
}