#version 430 core
out vec4 f_color;

in DRILL_OUT
{
   vec3 position_original;
   vec3 position;
   vec3 position_bigger;
   vec3 normal;
   vec2 texCoord;
   float seed;
} f_in;

uniform float z_rotation;
uniform bool slow;

float hash(float n) {
    return -1+fract(sin(n)*678.9)*2;
}

float EllipsoidDis2(vec3 center, vec3 point) {
    //return (point.x*point.x)/(center.x*center.x)+(point.y*point.y)/(center.y*center.y)+(point.z*point.z)/(center.z*center.z);
    return (center.x-point.x)*(center.x-point.x)+(center.y-point.y)*(center.y-point.y)+2500*(center.z-point.z)*(center.z-point.z);
}

void main()
{
    const vec4 black = vec4 (0.03,0.03,0.03,1.0);

    vec3 pos = f_in.position_original;
    float section = round(pos.z*200);

    vec3 center;
    float r = hash(section);
    if(slow){
        center = vec3(0.4*cos(-2*z_rotation+3*r)+r*0.1,0.4*sin(-2*z_rotation+3*r)+r*0.1,float(round(section))/200.0);
    }else{
        float r = hash(f_in.seed+section);
        center = vec3(0.4*cos(r*3.14)+r*0.1,0.4*sin(r*3.14)+r*0.1,float(round(section))/200.0);
    }

    if(pos.z>0.45)
        discard;
    if(EllipsoidDis2(pos,center) < 0.15+(pos.z+0.5)*0.19)
        f_color = black;
    else
        discard;
}