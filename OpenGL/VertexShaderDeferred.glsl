#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
}vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    mat3 NormalMatrix = mat3(transpose(inverse(model)));
    vs_out.Normal = NormalMatrix * aNormal;
    vs_out.TexCoord = aTexCoord;
}