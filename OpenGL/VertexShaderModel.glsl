#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

// layout (std140) uniform Matrices
// {
uniform    mat4 view;
uniform    mat4 projection;
// };

// out VS_OUT
// {
//     vec3 Position;
//     vec3 Normal;
//     vec2 TexCoord;
// } vs_out;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    // vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    // vs_out.Position = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // vs_out.TexCoord = aTexCoord;

    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0f));
    TexCoord = aTexCoord;


}