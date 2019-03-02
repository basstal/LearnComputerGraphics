#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
}vs_out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform bool invertedNormals;
void main()
{
    vs_out.FragPos = vec3(view * model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    mat3 NormalMatrix = mat3(transpose(inverse(view * model)));
    vec3 nm = aNormal;
    if (invertedNormals)
        nm = - aNormal;
    vs_out.Normal = NormalMatrix * nm;
    vs_out.TexCoord = aTexCoord;
}