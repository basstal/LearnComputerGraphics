#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// uniform mat4 model;
// uniform mat4 projection;
// uniform mat4 view;

out VS_OUT
{
    vec2 TexCoord;
    // vec3 Normal;
}vs_out;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
    // gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // vs_out.TexCoord = aTexCoord;
    // mat3 NormalMatrix = mat3(transpose(inverse(model)));
    // vs_out.Normal = NormalMatrix * aNormal;
}