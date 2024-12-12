#version 430 core
out vec4 f_color;

in float actualHeight;
in vec2 samplePos;

uniform bool useModel = false;
uniform sampler2D islandHeight;

void main()
{   
    if(useModel){
        float groundHeight = texture(islandHeight,samplePos).x+300;
        if(abs(actualHeight-groundHeight)>5)
            discard;
        f_color = vec4(0.15,0.15,0.15,1.0);
    }
    else{
        f_color = vec4(0.15,0.15,0.15,1.0);
    }
}