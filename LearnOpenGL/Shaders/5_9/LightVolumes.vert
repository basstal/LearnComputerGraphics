#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec3 NormalModel;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Position; // lightVolumes 顶点的世界坐标
    mat4 projection;
    mat4 view;
    mat4 model;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.NormalModel = aNormal;
    vs_out.Position = aPosition;
    vs_out.view = view;
    vs_out.projection = projection;
    vs_out.model = model;
}