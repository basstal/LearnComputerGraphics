#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

out VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragLightSpacePos;
}  vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
    mat3 NormalMatrix = mat3(transpose(inverse(model)));
    vs_out.Normal = NormalMatrix * aNormal;
    vs_out.FragLightSpacePos = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
}