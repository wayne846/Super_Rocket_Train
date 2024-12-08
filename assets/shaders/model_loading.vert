#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 normal;
out vec3 position;

uniform mat4 model;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

void main()
{
    TexCoords = aTexCoords;   
    normal = mat3(transpose(inverse(model))) * aNormal;
    position = (model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}