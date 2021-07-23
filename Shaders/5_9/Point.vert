#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

out VS_OUT
{
    mat4 projection;
    vec3 normal;
    vec3 position;
}vs_out;

void main()
{
    gl_Position = view * model * vec4(aPosition, 1.0f);
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.projection = projection;
}
