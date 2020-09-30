#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;

};

uniform Material material;

in vec2 TexCoords;
void main()
{
    FragColor = texture(material.diffuse, TexCoords);
}