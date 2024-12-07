#version 430 core
layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoordIn;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 normalMatrix;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out vec3 position_inner;

void main()
{
    gl_Position = projection * view * model * vec4(positionIn, 1);
    position_inner = gl_Position.xyz;
}