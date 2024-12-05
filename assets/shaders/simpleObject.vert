#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoordIn;

uniform mat4 model;
uniform mat4 normalMatrix;

uniform bool useImage;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texCoord;
} v_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1);
    v_out.position = (model * vec4(position, 1)).xyz;
    v_out.normal = mat3(normalMatrix) * normal;
    vec2 texCoord = vec2(0,0);
    if(useImage){
        texCoord = texCoordIn;
    }
    v_out.texCoord = texCoord;
}