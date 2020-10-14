#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

out VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    mat4 projection;
}vs_out;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.Normal = normalize(normalMatrix * aNormal);
    vs_out.projection = projection;
}