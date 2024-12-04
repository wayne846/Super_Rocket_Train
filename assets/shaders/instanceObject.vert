#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 model;
layout (location = 6) in mat4 normalMatrix;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
} v_out;

void main()
{
    //mat4 normalMatrix = transpose(inverse(model));
    gl_Position = projection * view * model * vec4(position, 1);
    v_out.position = (model * vec4(position, 1)).xyz;
    v_out.normal = mat3(normalMatrix) * normal;
}