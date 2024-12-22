#version 430 core
out vec4 f_color;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float gamma = 2.0;

void main()
{    
    f_color = texture(skybox, TexCoords);
    f_color.rgb = pow(f_color.rgb, vec3(1.0/gamma));
}