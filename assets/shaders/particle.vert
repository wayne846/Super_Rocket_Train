#version 430 core
layout (location = 0) in vec3 instancePos;  // 采竚
layout (location = 1) in vec3 instanceColor; // 采肅︹
layout (location = 2) in float instanceSize; // 采

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out vec3 fragColor;

void main() {
    const float scale = 1000.0f;

    gl_Position = projection * view * vec4(instancePos, 1.0); // ㄏノ采竚程沧竚
    gl_PointSize = instanceSize * scale / gl_Position.z;          // 砞﹚采
    fragColor = instanceColor;            // 肚患肅︹琿帝︹竟
}