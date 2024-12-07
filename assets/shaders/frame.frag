#version 430 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{ 
    vec2 PixelCoords;
    PixelCoords.x = round(TexCoords.x*100)/100;
    if(PixelCoords.x == 0.0)
        PixelCoords.x = 0.005;
    if(PixelCoords.x == 1.0)
        PixelCoords.x = 0.995;
    PixelCoords.y = round(TexCoords.y*100)/100;
    if(PixelCoords.y == 0.0)
        PixelCoords.y = 0.005;
    if(PixelCoords.y == 1.0)
        PixelCoords.y = 0.995;
    FragColor = texture(screenTexture, PixelCoords);
}