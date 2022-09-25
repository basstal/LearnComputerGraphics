#version 330 core
layout (location = 0) in vec3 aPosition;
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
    mat4 model;
    vec3 Position;
}vs_out;

void main()
{
    gl_Position = view * model * vec4(aPosition, 1.0);
    vs_out.Normal = aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.projection = projection;
    vs_out.model = model;
    vs_out.Position = aPosition;
}
