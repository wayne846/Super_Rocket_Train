#version 430 core
in vec2 TexCoords;
in vec2 screen_dir;

out vec4 FragColor;

uniform sampler2D bg;
uniform float t;

void main()
{ 
    //vec2 screen_dir=vec2 (0,1);
    vec2 standardCoords = TexCoords*2-vec2(1,1);
    vec2 rotateStandardCoords = vec2(standardCoords.x*screen_dir.x-standardCoords.y*screen_dir.y,
                                     standardCoords.x*screen_dir.y+standardCoords.y*screen_dir.x);
    vec2 realCoords = rotateStandardCoords/2+0.5;
    FragColor = texture(bg, vec2((realCoords.x+0.6*t)*0.01,realCoords.y));
}