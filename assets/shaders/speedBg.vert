#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

uniform vec3 drill_dir;

out vec2 TexCoords;
out vec2 screen_dir;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0); 
    TexCoords = aTexCoords;
    screen_dir = (view * vec4(drill_dir,0.0)).xy;
    screen_dir = normalize(screen_dir);
    screen_dir.x = -1*screen_dir.x;
}