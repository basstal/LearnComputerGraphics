#version 330 core
out float FragColor;

uniform sampler2D ssaoInput;

in vec2 TexCoord;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            result += texture(ssaoInput, TexCoord + offset).r;
        }
    }
    FragColor = result / (4.0 * 4.0);
}