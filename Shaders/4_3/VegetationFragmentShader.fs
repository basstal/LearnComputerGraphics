#version 330 core
out vec4 FragColor;

uniform sampler2D vegetationTex;

in vec2 TexCoords;
void main()
{
    vec4 texColor = texture(vegetationTex, TexCoords);
    if (texColor.a < 0.1)
        discard;
    FragColor = texColor;
}