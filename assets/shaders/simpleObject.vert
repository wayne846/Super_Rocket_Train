#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out V_OUT
{
   vec3 position;
   vec3 normal;
} v_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1);
    v_out.position = (model * vec4(position, 1)).xyz;
    v_out.normal = mat3(transpose(inverse(model))) * normal;
}