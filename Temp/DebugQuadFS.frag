#version 330 core
out vec4 FragColor;

uniform sampler2D renderTexture;
uniform bool onlyAlpha;

in vec2 TexCoords;

void main()
{
    if (onlyAlpha)
    {
        vec4 color = texture(renderTexture, TexCoords);
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        FragColor = vec4(average, average, average, color.a);
    }
    else
        FragColor = vec4(texture(renderTexture, TexCoords).rgb, 1.0);
}