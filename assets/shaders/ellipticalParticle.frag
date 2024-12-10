#version 430 core

in vec3 fragColor;
in vec2 velocity2;
in float eccentricity;

out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord*2 - vec2(1,1);

    vec2 rotateCoords = vec2(coord.x*velocity2.x-coord.y*velocity2.y,
                             coord.x*velocity2.y+coord.y*velocity2.x);
    float dis = rotateCoords.x*rotateCoords.x+rotateCoords.y*rotateCoords.y/0.01;
    if (dis > 1) {
        discard;
    }else{
        FragColor = vec4 (fragColor,1-((dis+0.1)/1.1));
    }
}
