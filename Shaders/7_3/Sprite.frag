#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D sprite;
uniform vec3 spriteColor;

void main()
{
    color = vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
}