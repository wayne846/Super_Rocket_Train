#version 430 core
layout (location = 0) in vec3 instancePos;  // 采竚
layout (location = 1) in vec3 instanceColor; // 采肅︹
layout (location = 2) in float instanceSize; // 采
layout (location = 3) in vec3 velocity;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out vec3 fragColor;
out vec2 velocity2;
out float eccentricity;

void main() {
    const float scale = 1000.0f;

    gl_Position = projection * view * vec4(instancePos, 1.0); // ㄏノ采竚程沧竚
    gl_PointSize = instanceSize * scale / gl_Position.z;          // 砞﹚采
    if(projection[3]==vec4(0,0,0,1))      // is Ortho
        gl_PointSize = instanceSize * scale / 300.0f;
    fragColor = instanceColor;            // 肚患肅︹琿帝︹竟

    vec3 velocity3 = (projection * view * vec4(velocity, 0.0)).xyz;
    velocity2 = normalize(velocity3.xy);
}