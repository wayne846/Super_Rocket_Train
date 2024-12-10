#version 430 core
in vec2 TexCoords;

out vec4 FragColor;

uniform float time;
uniform bool useCrosshair = false;
uniform bool bulletTime = false;
uniform bool useSpiral = false;

uniform float screenAspectRatio=1.0;
uniform sampler2D screenTexture;
uniform sampler2D crosshairTexture;
uniform sampler2D whiteLineTexture;

vec4 grayScale(vec4 color);
vec4 reduceSaturation(vec4 color, float strength);

void main()
{ 
    const vec4 black = vec4 (0.0,0.0,0.0,1.0);
    const vec4 white = vec4 (1.0,1.0,1.0,1.0);
    const vec4 lightGreen = vec4 (0.37,0.87,0.69,1.0);

    vec4 color;

    color = texture(screenTexture, TexCoords);
    if(useCrosshair){
        vec2 scaledCoords = vec2((TexCoords.x-0.5)* screenAspectRatio*0.5+0.5, TexCoords.y );
        vec4 temp_color = texture(crosshairTexture, scaledCoords);
        color = mix(color,temp_color,temp_color.w);
    }

    // bullet time
    if (bulletTime){
        color =reduceSaturation(color,0.8);
        float r = abs((TexCoords.x-0.5)*(TexCoords.x-0.5)*4+(TexCoords.y-0.5)*(TexCoords.y-0.5)*4);
        color = mix(color,black,(r-0.5)*0.5);
        if(texture(whiteLineTexture, vec2(TexCoords.x+0.0017f,TexCoords.y))!=texture(whiteLineTexture, vec2(TexCoords.x,TexCoords.y+0.0017f)))
            color = white;
    }

    // green border
    if(useSpiral){
        texture(whiteLineTexture, vec2(TexCoords.x+0.0017f,TexCoords.y));
        float r = abs((TexCoords.x-0.5)*(TexCoords.x-0.5)*4+(TexCoords.y-0.5)*(TexCoords.y-0.5)*4);
        color = mix(color,lightGreen,(r-0.5)*0.5*abs(sin(time*6.28*0.015)));
    }
    FragColor = color;
}

vec4 grayScale(vec4 color){
    float avg = (0.299*color.r+0.587*color.g+0.114*color.b);
    return vec4 (avg,avg,avg,1.0);
}

vec4 reduceSaturation(vec4 color, float strength){
    float avg = (0.299*color.r+0.587*color.g+0.114*color.b);
    return vec4 (mix(color.r,avg,strength),mix(color.g,avg,strength),mix(color.b,avg,strength),1.0);
}
