#version 430 core
out vec4 f_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

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
  
in V_OUT
{
   vec3 position;
   vec3 normal;
} f_in;

uniform vec3 eyePosition;
uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 4  
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 eyeDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 eyeDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 eyeDir);

void main()
{   
    // properties
    vec3 norm = normalize(f_in.normal);
    vec3 eyeDir = normalize(eyePosition - f_in.position);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, eyeDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLights[i], norm, f_in.position, eyeDir);    
    }
        
    // phase 3: Spot light
    for(int i = 0; i < NR_SPOT_LIGHTS; i++){
        result += CalcSpotLight(spotLights[i], norm, f_in.position, eyeDir);
    }  
    
    f_color = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 eyeDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 eyeDir)
{
    vec3 lightDir = normalize(light.position - position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 eyeDir){
    vec3 lightDir = normalize(light.position - position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}