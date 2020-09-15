#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

layout (std140) uniform Matrices
{
    // 这里的顺序很重要  切记！！！！
    mat4 projection;
    mat4 view;
};

out VS_OUT
{
    // vec3 fragPos;
    vec3 normal;
    vec2 TexCoord;
} vs_out;

// out vec3 fragPos;
// out vec3 normal;
// out vec2 TexCoord;

void main()
{
    // vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    // vs_out.fragPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    mat3 normalMatrix  = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(projection * vec4(normalMatrix  * aNormal, 1.0f)));
    vs_out.TexCoord = aTexCoord;

    // normal = mat3(transpose(inverse(model))) * aNormal;
    // fragPos = vec3(model * vec4(aPos, 1.0f));
    // TexCoord = aTexCoord;


}