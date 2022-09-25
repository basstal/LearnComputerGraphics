#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 TexCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(vertex, 1.0f);
    TexCoords = TexCoord;
}