#version 330 core
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 TexCoord;
void main()
{
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
}