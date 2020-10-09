#version 330 core
out vec4 FragColor;

uniform sampler2D renderTexture;

in vec2 TexCoord;

void main()
{
    FragColor = texture(renderTexture, TexCoord);
}