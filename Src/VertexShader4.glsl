#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Position;
// out vec3 Normal;
out vec2 TexCoord;

void main()
{
    // Normal = mat3(transpose(inverse(model))) * aNormal;
    
    Position = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    gl_PointSize = gl_Position.z;
    TexCoord = aTexCoord;
}