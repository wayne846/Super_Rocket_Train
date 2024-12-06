#version 430 core
layout (location = 0) in vec3 instancePos;  // �ɤl����m
layout (location = 1) in vec3 instanceColor; // �ɤl���C��
layout (location = 2) in float instanceSize; // �ɤl���j�p

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

out vec3 fragColor;

void main() {
    const float scale = 1000.0f;

    gl_Position = projection * view * vec4(instancePos, 1.0); // �ϥβɤl����m�@���̲צ�m
    gl_PointSize = instanceSize * scale / gl_Position.z;          // �]�w�ɤl���j�p
    fragColor = instanceColor;            // �ǻ��C�����q�ۦ⾹
}