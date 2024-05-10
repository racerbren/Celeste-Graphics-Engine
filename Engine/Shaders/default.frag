#version 330

layout (location=0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 FragPosLightSpace;
  
uniform sampler2D ourTexture;
uniform sampler2D shadowMap;
uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;
};
uniform DirectionalLight dirLight;

struct PointLight
{
    vec3 position;

    //Floats used for attenuation
    float linear;
    float quadratic;
};
const int numPointLights = 1;
uniform PointLight pointLights[numPointLights];

//(ambient x, diffuse y , specular z, shininess w)
uniform vec4 material;

vec4 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec4 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);
float calculateShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection);

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

    //Calculate shadows for directional light
    float shadows = calculateShadows(FragPosLightSpace, normal, lightDirection);
    
    //Return effects of directional light
    return vec4((ambient + (1.0 - shadows) * (diffuse + specular)), 1.0);
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

float calculateShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection)
{
    //Perspective divide light space fragment in clip space to projection coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //Transform projection coordinates to range [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    //Sample the shadow map to get the closest depth value to the light
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    //Get the depth of this fragment by sampling the z coordinate of the projection
    float currentDepth = projCoords.z;
    
    //Create a small bias to offset the depths of the shadow map
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    //Check if the current depth or closest depth is closer
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    //For fragments outside of the far plane that should not be in shadow, set equal to 0 so they are always lit
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}