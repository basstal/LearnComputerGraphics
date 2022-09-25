#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
}