#version 430 core

in vec3 position;
in float alpha;

out vec4 FragColor;

void main()
{
    if(alpha<=0.5){
        FragColor.rgb = mix(vec3(1.0,1.0,0.0),vec3(1.0,0.0,0.0),alpha*2);
    }else{
        FragColor.rgb = mix(vec3(1.0,0.0,0.0),vec3(0.5,0.5,0.5),(alpha-0.5)*2);
    }
}
