#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoordIn;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 normalMatrix;

uniform bool useImage;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out DRILL_OUT
{
   vec3 position_original;
   vec3 position;
   vec3 position_bigger;
   vec3 normal;
   vec2 texCoord;
   float seed;
} v_out;

float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

void main()
{
    mat4 bigger_model = model;
    bigger_model[3].x *= 0.9;
    bigger_model[3].y *= 0.9;
    bigger_model[3].z *= 0.9;
    bigger_model[3].w *= 0.9;
    v_out.seed = bigger_model[0].x;
    gl_Position = projection * view * bigger_model * vec4(position, 1);
    v_out.position = (model * vec4(position, 1)).xyz;
    v_out.position_bigger = (bigger_model * vec4(position, 1)).xyz;
    v_out.position_original = position;
    v_out.normal = mat3(normalMatrix) * normal;
    vec2 texCoord = vec2(0,0);
    if(useImage){
        texCoord = texCoordIn;
    }
    v_out.texCoord = texCoord;
}