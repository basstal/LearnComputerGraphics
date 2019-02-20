#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

// uniform vec2 offset[100];

out vec3 Color;

void main()
{
    vec2 pos = aPos * (gl_InstanceID / 100.0) + aOffset;
    gl_Position = projection * view * model * vec4(pos.x, pos.y, 0.0f, 1.0f);
    Color = aColor;
}