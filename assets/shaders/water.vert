#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform sampler2D heightMap;
uniform sampler2D normalMap;

uniform mat4 model;
uniform mat4 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

out V_OUT {
    vec3 position;
    vec2 texCoords;
} v_out;

void main()
{
    const float heightMapStrength = 5;

    v_out.position = (model * vec4(position, 1)).xyz + vec3(0, 1, 0) * texture(heightMap, texCoords).r * heightMapStrength;
    v_out.texCoords = texCoords;
    gl_Position = projection * view * vec4(v_out.position, 1);
}