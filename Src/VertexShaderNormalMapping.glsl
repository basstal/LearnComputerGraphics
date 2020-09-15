#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 model;

uniform mat4 projection;
uniform mat4 view;

out VS_OUT
{
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec2 TexCoord;
}vs_out;


void main()
{
    mat3 NormalMatrix = mat3(transpose(inverse(model)));

    vec3 T = normalize(NormalMatrix * aTangent);
    vec3 N = normalize(NormalMatrix * aNormal);
    // vec3 B = normalize(NormalMatrix * aBitangent);

    // Gram-Schmidt process
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 FragPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // vs_out.Normal = NormalMatrix * aNormal;
    vs_out.TexCoord = aTexCoord;
    vs_out.TangentFragPos = TBN * FragPos;
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    
}