#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoordIn;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 normalMatrix;

uniform bool useModel = false;
uniform sampler2D islandHeight;

out float actualHeight;
out vec2 samplePos;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

void main()
{
    vec4 worldPos = model * vec4(position, 1);
    if(!useModel){
        if(worldPos.y>=0)
            worldPos.y=0.1;
        else
            worldPos.y=-0.5;
    }else{
        samplePos = worldPos.xz/800+vec2(0.5,0.5);
        float groundHeight = texture(islandHeight,samplePos).x+300;
        if(worldPos.y>groundHeight){
            worldPos.y=groundHeight+1;
        }else{
            worldPos.y= -100;
        }
        actualHeight = worldPos.y;
    }
    gl_Position = projection * view * worldPos;
}