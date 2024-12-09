#version 430 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D bg;

uniform float rotation = -3.14*0.3;
uniform float t;



void main()
{ 
    vec2 pos = vec2((TexCoords*2-vec2(1,1)).x*cos(rotation)+(TexCoords*2-vec2(1,1)).y*sin(rotation),
                    (TexCoords*2-vec2(1,1)).x*-sin(rotation)+(TexCoords*2-vec2(1,1)).y*cos(rotation));
    FragColor = texture(bg, vec2(((pos/2+0.5).x+0.6*t)*0.01,(pos/2+0.5).y));
}