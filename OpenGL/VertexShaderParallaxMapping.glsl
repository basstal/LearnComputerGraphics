#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT
{
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightPos;
    vec3 TangentNormal;
    vec2 TexCoord;
    vec3 FragPos;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 B = normalize(mat3(model) * aBitangent);
    // T = normalize(T - dot(T, N) * N);
    // vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
    vs_out.TangentNormal = TBN * aNormal;
}