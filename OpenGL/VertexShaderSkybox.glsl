#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.xyww);
    texCoord = aPos;
}