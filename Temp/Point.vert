#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in float legalPoints;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT{
    float legalPoints;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    vs_out.legalPoints = legalPoints;
}