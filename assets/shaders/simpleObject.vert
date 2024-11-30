#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout(location = 2) in vec4 modelMatrixCol0;
layout(location = 3) in vec4 modelMatrixCol1;
layout(location = 4) in vec4 modelMatrixCol2;
layout(location = 5) in vec4 modelMatrixCol3;

uniform mat4 view;
uniform mat4 projection;

out V_OUT
{
   vec3 position;
   vec3 normal;
} v_out;

void main()
{
    mat4 model = mat4(modelMatrixCol0, modelMatrixCol1, modelMatrixCol2, modelMatrixCol3);
    mat4 normalMatrix = transpose(inverse(model));
    gl_Position = projection * view * model * vec4(position, 1);
    v_out.position = (model * vec4(position, 1)).xyz;
    v_out.normal = mat3(normalMatrix) * normal;
}