#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 lightParams;
layout (location = 4) in vec3 lightPosition;
layout (location = 5) in vec3 lightColor;
layout (location = 6) in mat4 model;


uniform mat4 projection;
uniform mat4 view;

out VS_OUT
{
    vec3 NormalModel;
    vec3 Position; // lightVolumes 顶点的世界坐标
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 lightPosition;
    vec3 lightColor;
    float lightRadius;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    vs_out.NormalModel = aNormal;
    vs_out.Position = aPosition;
    vs_out.view = view;
    vs_out.projection = projection;
    vs_out.model = model;
    vs_out.lightColor = lightColor;
    vs_out.lightPosition = lightPosition;
    vs_out.lightRadius = lightParams.x;
}