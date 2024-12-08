#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 position;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff; //this will be set by cos(theta)
    float outerCutOff; //this will be set by cos(theta)

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 eyePosition;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 4  
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform float gamma;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 eyeDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 eyeDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 eyeDir);

const float shininess = 99999.0f;

void main()
{    
    // properties
    vec3 norm = normalize(normal);
    vec3 eyeDir = normalize(eyePosition - position);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, eyeDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLights[i], norm, position, eyeDir);    
    }
        
    // phase 3: Spot light
    for(int i = 0; i < NR_SPOT_LIGHTS; i++){
        result += CalcSpotLight(spotLights[i], norm, position, eyeDir);
    }
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 eyeDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + eyeDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    //return (ambient + diffuse + specular);
    return (ambient + diffuse);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 eyeDir)
{
    vec3 lightDir = normalize(light.position - position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + eyeDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // attenuation
    float distance    = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    //return (ambient + diffuse + specular);
    return (ambient + diffuse);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 eyeDir){
    vec3 lightDir = normalize(light.position - position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + eyeDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // attenuation
    float distance    = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    //return (ambient + diffuse + specular);
    return (ambient + diffuse);
}