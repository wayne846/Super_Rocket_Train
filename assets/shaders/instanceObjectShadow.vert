#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoordIn;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 normalMatrix;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

void main()
{
    vec4 worldPos = model * vec4(position, 1);
    if(worldPos.y>=0)
        worldPos.y=0.1;
    else
        worldPos.y=-0.5;
    gl_Position = projection * view * worldPos;
}