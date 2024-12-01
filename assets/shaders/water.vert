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
    vec3 normal;
    vec2 texCoords;
} v_out;

void main()
{
    mat3 rotate = mat3(
        1.0f, 0.0f ,0.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f
    );

    //get normal in world space
    vec3 normal = normalize(rotate * texture(normalMap, texCoords).xyz);
    normal = mat3(normalMatrix) * normal;

    v_out.position = (model * vec4(position, 1)).xyz + normal * texture(heightMap, texCoords).r;
    v_out.normal = normal;
    v_out.texCoords = texCoords;
    gl_Position = projection * view * vec4(v_out.position, 1);
}