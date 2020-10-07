#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D windowTex;

void main()
{
    FragColor = texture(windowTex, TexCoords);
}