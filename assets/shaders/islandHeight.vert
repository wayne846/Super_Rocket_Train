#version 430 core
layout (location = 0) in vec3 aPos;

out float y;

uniform mat4 model;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

void main()
{
    vec4 worldPos = (model * vec4(aPos, 1.0));
    y = worldPos.y;
    worldPos/=400;
    gl_Position = vec4(worldPos.x,worldPos.z,-worldPos.y,1.0);
}