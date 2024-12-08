#version 430 core
in vec3 position_inner;

out vec4 f_color;

void main()
{   
    f_color = vec4(position_inner.z / 500.0, position_inner.z / 500.0, position_inner.z /500.0 , 1.0);
}