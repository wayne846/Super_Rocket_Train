#version 430 core
layout (location = 0) in vec3 instancePos;  // �ɤl����m
layout (location = 1) in vec3 instanceColor; // �ɤl���C��
layout (location = 2) in float instanceSize; // �ɤl���j�p
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

    gl_Position = projection * view * vec4(instancePos, 1.0); // �ϥβɤl����m�@���̲צ�m
    gl_PointSize = instanceSize * scale / gl_Position.z;          // �]�w�ɤl���j�p
    if(projection[3]==vec4(0,0,0,1))      // is Ortho
        gl_PointSize = instanceSize * scale / 300.0f;
    fragColor = instanceColor;            // �ǻ��C�����q�ۦ⾹

    vec3 velocity3 = (projection * view * vec4(velocity, 0.0)).xyz;
    velocity2 = normalize(velocity3.xy);
}