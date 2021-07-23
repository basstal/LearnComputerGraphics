#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;


void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    // Note the xyww trick here that ensures the depth value of the rendered cube fragments always end up at 1.0, the maximum depth value
    gl_Position = clipPos.xyww;
}